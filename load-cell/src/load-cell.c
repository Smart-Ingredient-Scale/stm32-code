#include "stm32f4xx.h"
#include <stdint.h>
#include "load-cell.h"
#include "clock.h"
#include "gpio.h"

#ifndef CPU_CLK
#define CPU_CLK 16000000
#endif /* CPU_CLK */
#define SD_OUT_PIN      8       /* PB8 for HX711 serial data out */
#define TIM4_PSC_CLK    1000    /* 1000 Hz prescaler clock */
#define UEV_FREQUENCY   80      /* 80 Hz UEV event freqeuncy */

// TODO: Override the ISR for TIM4 to sample a digital pin and count
// number of pwm-generated clocks to deactivate the PWM but make sure
// to keep checking if the GPIO pin has been driven low (more data
// available and then check the line again

/* (1) Channel A 128 gain = 25 clks : all posedge clocks sent to HX711
 * (2) Channel B 32 gain = 26 clks
 * (3) Channel A 64 gain = 27 clks */
static enum HX711_Clk_Config {CHA_128_GAIN=25, CHB_32_GAIN=26, CHA_64_GAIN=27};
static enum HX711_Data_Status {DATA_READY, DATA_UNAVAILABLE};
static enum HX711_Retrieval_Status {RECEIVING_DATA, CONFIGURING_CHANNEL, INACTIVE};


/* Function declarations */
static void config_pb9_pwm(void);
static void config_pb8_digitalIO(void);
static void config_tim4_ch4(void);

/* Global variables */
static enum HX711_Retrieval_Status retrievalStatus;
static enum HX711_Data_Status icStatus;
static const enum HX711_Clk_Config clkMode = CHA_64_GAIN; /* doesn't change */

/* Set PB9 for AF = 2 (TIM_CH4 output pin) */
static void config_pb9_pwm(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* configure the GPIO pin for alternate function */
    GPIOB->MODER &= ~GPIO_MODER_MODER9;
    GPIOB->MODER |= GPIO_MODER_MODER9_1;

    GPIOB->OTYPER &= ~GPIO_OTYPER_IDR_9; /* push-pull output pin */

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR9; /* no pull-up or pull-down resistor */

    GPIOB->AFR[1] &= ~0xF0;  /* clear the PB9 AF bits */
    GPIOB->AFR[1] |= 0x20;    /* define PB9 for AF 2 */
}

/* Configure PB8 for digital IO. Used to read the digital line
 * on the HX711 IC via bit-banging. Called by TIM4 ISR. */
static void config_pb8_digitialIO(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* configure PB8 for digital input */
    GPIOB->MODER &= ~GPIO_MODER_MODER8;

    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR8; /* no biasing resistor */
}

/* Configure TIM4 for HX711 IC clock generation using TIM4 CH4 
 * on PB9 GPIO pin. Gets fed with the APB1 bus clock (32MHz) */
static void config_tim4_ch4(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    TIM4->CR1 = 0;
    TIM4->CR1 |= TIM_CR1_ARPE;  /* allow for dynamic changing of the ARR (adjust PWM duty cycle) */

    /* Divide down the 64MHz CPU clk to get the proper ISR firing rate */
    TIM4->PSC = APB1_CLK / TIM4_PSC_CLK - 1;
    TIM4->ARR = 1000 - 1; // TODO: change to UEV_FREQUENCY

    /* reset values for triggers and other irrelevant settings */
    TIM4->CR2 = 0;
    TIM4->SMCR = 0;

    /* (1) Update event ISR will sample the serial data line */
    TIM4->DIER |= (TIM_DIER_UIE);

    /* Configure TIM4 channel 4 PWM Mode 1 (see frm pg. 464-467) */
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC4S  |  /* configure CCR4 CH4 for output */
                     TIM_CCMR2_OC4FE |  /* configure CCR4 for normal-speed operation */
                     TIM_CCMR2_OC4PE |  /* clear the preload enable for CH4 */
                     TIM_CCMR2_OC4M  |  /* clear the mode bits (PWM mode for example) */
                     TIM_CCMR2_OC4CE);  /* do NOT change reset voltage reference */

    TIM4->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | /* PWM Mode 2 */
                    TIM_CCMR2_OC4PE);                     /* in case CCR changes, do it safely */

    TIM4->CCER &= ~(TIM_CCER_CC4NP |    /* do NOT invert output */
                    TIM_CCER_CC4P);     /* treat output at as active high */

    TIM4->CCER |= TIM_CCER_CC4E;        /* mux the CC4 output to the GPIO (PB9) pin */

    TIM4->CCR4 = (TIM4->ARR >> 1);  /* 50% duty cycle clock */

    /* Enable the interrrupts in the NVIC */
    NVIC_EnableIRQ(TIM4_IRQn);
    NVIC_SetPriority(TIM4_IRQn, 0);

    /* Enable the timer to begin counting */
    TIM4->CR1 |= TIM_CR1_CEN;
}

volatile uint8_t pa5outStatus = 0;
/* Redefined the TIM4 IRQ handler */
void TIM4_IRQHandler(void) {
    TIM4->SR &= ~(TIM_SR_UIF);

	if(pa5outStatus == 1)
        clear_pa5();
    else
        set_pa5();

    pa5outStatus ^= 1;
}

/* Configure the TIM4 UEV ISR for the 80Hz clock supplied. Use the 
* statically-global HX711_Biasing_Type data structure to determine
* how many clocks to generate (specifies the gain and input channel).  */


/* Read from the active-low serial data output port of the HX711
 * IC to determine if data is ready. Read on PB8. */
static enum HX711_Data_Status get_hx711_data_status(void) {
    if(GPIOB->IDR & (1 << SD_OUT_PIN)) {
        return DATA_UNAVAILABLE;
    }

    else {
        return DATA_READY;
    }
}

/* Initializes the load cell and supporting peripherals */
void load_cell_init(void) {
    config_pb8_digitialIO();
    config_pb9_pwm();
    config_tim4_ch4();
}
