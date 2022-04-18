#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include <stdint.h>
#include "load-cell.h"
#include "clock.h"
#include "gpio.h"
#include "screen.h"

/* HX711 FSM state flags / values */
static enum HX711ChanConfig_t clks_per_sample = CHA_64_GAIN; /* doesn't change */
static enum Tim4IrqState_t state;
static enum HX711SamplingRate_t f_sample = HZ10; /* hard-code sampling rate */

/* Function declarations */
static void config_pb8_digitalIO(void);
static void config_pb9_pwm(void);
static void config_tim4_ch4(enum HX711SamplingRate_t);
static void update_sample(uint32_t newSampleBits);
static inline void disable_sdclk_pwm(void);
static inline void enable_sdclk_pwm(void);
static inline uint32_t hx711_data_bit(void);
static inline enum HX711DataStatus_t hx711_data_ready(void);

int32_t zeroADCVal = -64600;
extern units_t cur_display_unit;

/* Set PB9 for AF = 2 (TIM_CH4 output pin) since it will operate as
 * the clock used for getting data from the HX711 IC */
static void config_pb9_pwm(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* configure the GPIO pin for alternate function */
    GPIOB->MODER &= ~GPIO_MODER_MODER9;
    GPIOB->MODER |= GPIO_MODER_MODER9_1;

    GPIOB->OTYPER &= ~GPIO_OTYPER_IDR_9; /* push-pull output pin */

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR9; /* no pull-up or pull-down resistor */
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR9_1;

    GPIOB->AFR[1] &= ~0xF0;  /* clear the PB9 AF bits */
    GPIOB->AFR[1] |= 0x20;    /* define PB9 for AF 2 */
}

/* Configure PB8 for digital IO. Used to read the digital input line
 * on the HX711 IC via bit-banging. Called by TIM4 ISR. */
static void config_pb8_digitalIO(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* configure PB8 for digital input */
    GPIOB->MODER &= ~GPIO_MODER_MODER8;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR8; /* no biasing resistor */
}

/* Configure TIM4 for HX711 IC clock generation using TIM4 CH4 
 * on PB9 GPIO pin. Gets fed with the APB1 bus clock (32MHz) */
static void config_tim4_ch4(enum HX711SamplingRate_t fs) {
    f_sample = fs; /* update the global variable */
    
    /* enable the timer peripheral */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    TIM4->CR1 = 0;
    TIM4->CR1 |= TIM_CR1_ARPE;  /* allow for changing the UEV ISR
                                 * firing frequency */

    /* Divide down the 32MHz TIM4 clk to get proper ISR firing rate */
    TIM4->PSC = TIM4_CLK / TIM4_PSC_CLK - 1;
    TIM4->ARR = TIM4_PSC_CLK / (2 * fs) - 1; /* fire twice as fast as needed (20Hz or 160Hz) */

    /* reset values for triggers and other irrelevant settings */
    TIM4->CR2 = 0;
    TIM4->SMCR = 0;

    /* (1) Update event ISR will sample the serial data line */
    TIM4->DIER |= (TIM_DIER_UIE);

    /* Configure TIM4 channel 4 PWM Mode 2 (see frm pg. 464-467) */
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC4S  |  /* configure CCR4 CH4 for output */
                     TIM_CCMR2_OC4FE |  /* configure CCR4 for normal-speed operation */
                     TIM_CCMR2_OC4PE |  /* clear the preload enable for CH4 */
                     TIM_CCMR2_OC4M  |  /* clear the mode bits (PWM mode for example) */
                     TIM_CCMR2_OC4CE);  /* do NOT change reset voltage reference */

    TIM4->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | /* PWM Mode 2 */
                    TIM_CCMR2_OC4PE);                     /* in case CCR changes, do it safely */

    TIM4->CCER &= ~(TIM_CCER_CC4NP |    /* do NOT invert output */
                    TIM_CCER_CC4P);     /* treat output at as active high */

    TIM4->CCR4 = ((TIM4_PSC_CLK / SD_CLK_FREQEUNCY) >> 1);      /* 50% duty cycle of SDCLK */

    /* Enable the interrupts in the NVIC */
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0);
}

/* enable the load cell by activating timer 4 (init first) */
void load_cell_enable(void) {
    TIM4->CR1 |= TIM_CR1_CEN;
}

/* disable the load cell by activating timer 4 (init first) */
void load_cell_disable(void) {
    TIM4->CR1 &= ~(TIM_CR1_CEN);
}



static volatile uint8_t clk_count = 0;
static volatile uint32_t newestSample = 0;           /* last 24-bit ADC sample */
/* The TIM4 IRQ handler should fire at least 10kHz or else the
 * ADC will power down. Will poll to see if data is available
 * on the HX711 (SDATA = 0). If so, begins reading data and
 * updates the moving average. Will only fire at the negedge
 * of the SDCLK due to the ARR configuration of TIM4 */
void TIM4_IRQHandler(void) {
    TIM4->SR &= ~(TIM_SR_UIF); /* acknowledge the ISR */

    if(state == IDLE0) { /* check for available data on SDOUT */
        if(hx711_data_ready() == READY) {
            /* configure ISR for sampling at SDCLK rate */
            TIM4->ARR = TIM4_PSC_CLK / SD_CLK_FREQEUNCY - 1;
            state = IDLE1;
        }
    }

    /* going to activate SDCLK (PWM) and clears clk count and new sample */
    else if (state == IDLE1) {
        enable_sdclk_pwm();     /* activate PWM immediately upon UEV */
        state = RECEIVING_DATA;
        clk_count = 0;
        newestSample = 0;
    }

    /* read 24-bits of ADC information into newest sample MSB-first */
    else if(state == RECEIVING_DATA) {
        clk_count++;

        newestSample |= (hx711_data_bit() << (NUM_BITS_PER_SAMPLE - clk_count));

        if(clk_count == NUM_BITS_PER_SAMPLE) {
            state = CONFIGURING_CHANNEL; /* configure next ADC conversion */
            update_sample(newestSample);
        }
    }

    /* vary number of extra clocks to configure next channel read */
    else {
        if(++clk_count == clks_per_sample) {
            disable_sdclk_pwm();       /* disable SDCLK before a posedge */
            TIM4->ARR = TIM4_PSC_CLK / (2 * f_sample); /* change to lower sampling frequency */
            state = IDLE0;
        }
    }
}

/* Read from the active-low serial data output pin on the HX711
 * IC to determine if data is ready. Read on PB8. */
static inline enum HX711DataStatus_t hx711_data_ready(void) {
    if(GPIOB->IDR & (1 << SD_OUT_PIN)) { /* pin is high */
        return UNAVAILABLE;
    }

    else { /* pin is low */
        return READY;
    }
}

/* Return 32'b0 or 32'b1 depending on whether PB8 is set */
static inline volatile uint32_t hx711_data_bit(void) {
    return((GPIOB->IDR >> SD_OUT_PIN) & 0x1);
}

/* Initializes the load cell and supporting peripherals to
 * read data at the sampling rate. Will handle the protocol
 * format based upon chosen channel and gain and sampling
 * rate. This sampling rate & channel must match hardware. */
void load_cell_init(enum HX711SamplingRate_t fs, enum HX711ChanConfig_t chanCfg) {
    clks_per_sample = chanCfg; /* channel configuration determines number
                                * of SDCLKs after the 24 bits of
                                * ADC data is read. */
    /* configure GPIOs */
    config_pb8_digitalIO();
    config_pb9_pwm();
    
    /* configure the TIM4 periperal w/ proper sampling rate */
    config_tim4_ch4(fs);
}

/* enable TIM5 CH4 PWM output */
static inline void enable_sdclk_pwm(void) {
    TIM4->CCER |= (TIM_CCER_CC4E);
}

/* enable TIM5 CH4 PWM output */
static inline void disable_sdclk_pwm(void) {
    TIM4->CCER &= ~(TIM_CCER_CC4E);
}

HX711_Data_t adc; /* sample storage data structure */

/* Adds a 2-s complement ADC sample (24 bits long) into the 
 * sample storage average data structure. Updates the moving average. */
static void update_sample(uint32_t newSampleBits) {
    int32_t sampleToReplace = adc.samples[adc.nextUpdatedInd];

    int32_t newSample; /* signed 32-bit version of new sample */

    /* sign extend the new sample into a 32-bit 2's complement integer */
    if(newSampleBits & (1 << (NUM_BITS_PER_SAMPLE - 1))) {
        newSample = (int32_t) (newSampleBits |= 0xFF000000);
    }

    else {
    	newSample = (int32_t) newSampleBits;
    }

    /* remove the weight of the sample about to be replaced */
    adc.cumulativeSum -= sampleToReplace;

    /* add the weight of the sample to replace the old sample */
    adc.cumulativeSum += newSample;
    
    /* replace the sample in the array and update the index */
    adc.samples[adc.nextUpdatedInd] = newSample;
    if(++adc.nextUpdatedInd == NUM_STORED_SAMPLES) {
        adc.nextUpdatedInd = 0;
    }

    /* update the moving average */
    adc.movingAverage = adc.cumulativeSum >> NUM_STORED_SAMPLES_BITSHIFT;
}

uint32_t cur_density = 1;

// This function converts the int32_t ADC value from the HX711 to grams/ounces/etc. using arthmetic
int32_t convert(int32_t x)
{
    // zeroADCVal is a global variable and is set every time the TARE (zero) button is pressed
    x -= zeroADCVal;
    x = x * 1000 / 11200; // conversion factor from ADC value to grams (default)

    if(cur_display_unit == UNITS_OUNCES)
    {
        x = x * 100 / 2835; // converts from g to oz - divide by 28.35
    }
    else if(cur_display_unit == UNITS_POUNDS)
    {
        x = x / 454; // converts from g to lb - divide by 454
    }
    else if(cur_display_unit == UNITS_MILLILITERS)
    {
        x = x / cur_density; // ml = grams / density
    }

    return x;
}

void init_button()
{
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //enable port C
    GPIOC->MODER &= ~GPIO_MODER_MODER3; // Set PC3 to input
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR3;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR3_1; // Set PC3 to pull down
}

// Touch screen press (debounced)
void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) == SET)
    {
        // Disable EXTI line until debounce
        EXTI_InitTypeDef EXTI_InitStruct;
        EXTI_InitStruct.EXTI_Line = EXTI_Line3;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStruct.EXTI_LineCmd = DISABLE;
        EXTI_Init(&EXTI_InitStruct);

        /* Clear the EXTI line 3 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line3);

        zeroADCVal = adc.movingAverage;
        micro_wait(500);

        EXTI_InitStruct.EXTI_Line = EXTI_Line3;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStruct);
    }
}

void init_button_interrupt()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    // Select the input source pin for the EXTI line
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource3);

    // Select the mode(interrupt, event) and configure the trigger selection (Rising, falling or both)
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line3;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configure NVIC IRQ channel mapped to the EXTI line
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
