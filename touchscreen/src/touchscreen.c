#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#include "clock.h"
#include "touchscreen.h"

// Usage:
// Call init_ts() to init peripherals
// interrupts will occur to fill ts_xpos, ts_ypos, and ts_pressed which
// can be access in other .c files using the following notation:
//
// extern uint16_t ts_xpos;
// extern uint16_t ts_ypos;
// extern int      ts_pressed;
//
// ts_xpos will be between 0 and X_SIZE
// ts_ypos will be between 0 and Y_SIZE
// ts_pressed will be 1 when there is a new touch event =
//   (acts as a flag signaling that info needs to be processsed)




uint16_t ts_xpos;
uint16_t ts_ypos;
int      ts_pressed = 0;


// Internal Functions
static void setup_debounce_timer();
static void enable_debounce_timer();
static void init_ts_interrupt();
static void ts_config_pin_ts();


static int isBetween(uint16_t val, uint16_t low, uint16_t high) {
    if (val >= low && val <= high) {
        return 1;
    } else {
        return 0;
    }
}

int inBox(uint16_t x, uint16_t y, uint16_t x_low, uint16_t y_low, uint16_t x_high, uint16_t y_high) {
    if (isBetween(x, x_low, x_high) && isBetween(y, y_low, y_high)) {
        return 1;
    }
    return 0;
}


// Touch screen press (debounced)
// Triggered on down-presses and up-presses, using "!TS_YP_IDR" to filter only down-presses
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) == SET && !TS_YP_IDR)
    {
        ts_xpos = TS_IO_GetX();
        ts_ypos = TS_IO_GetY();
        ts_pressed = 1;

        ts_config_pin_ts(); //put pins back in ts formation

        // Disable EXTI line until debounce
        EXTI_InitTypeDef EXTI_InitStruct;
        EXTI_InitStruct.EXTI_Line = TS_YP_EXTI_LINE;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStruct.EXTI_LineCmd = DISABLE;
        EXTI_Init(&EXTI_InitStruct);

        enable_debounce_timer();

        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}


// Debounce timer, re-enables the interrupt for detecting a touch
void TIM4_IRQHandler(void)
{
    TIM4 -> SR &= ~TIM_SR_UIF; //Acknowledge
    TIM4 -> CR1 &= ~TIM_CR1_CEN; // disable

    // // Re_enable EXTI
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = TS_YP_EXTI_LINE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
}

static void setup_debounce_timer()
{
    RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN; //enable clock

    TIM4 -> CR1 &= ~TIM_CR1_CEN; // make sure timer is disabled

    TIM4 -> PSC = 1600-1;
    TIM4 -> ARR = 6000-1; //12000 - 0.4s

    TIM4 -> DIER |= TIM_DIER_UIE; //enable interrupt on update
    TIM4 -> CR1 &= ~TIM_CR1_CEN; // disable timer

    NVIC -> ISER[0] |= 1<<TIM4_IRQn; //enable the interrupt to be accepted by NVIC ISER
}

static void enable_debounce_timer()
{
    TIM4 -> CR1 |= TIM_CR1_CEN; // enable
}


static void init_ts_interrupt()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Select the input source pin for the EXTI line
    SYSCFG_EXTILineConfig(TS_YP_EXTI_PORT, TS_YP_EXTI_PIN);

    // Select the mode(interrupt, event) and configure the trigger selection (Rising, falling or both)
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = TS_YP_EXTI_LINE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configure NVIC IRQ channel mapped to the EXTI line
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void ts_config_pin_ts()
{
    TS_YP_MODE_INPUT; /* YP = D_INPUT */
    TS_YM_MODE_INPUT; /* YM = D_INPUT */
    TS_XP_MODE_OUTPUT;
    TS_XM_MODE_OUTPUT;
    TS_XP_LOW; /* XP = 0 */
    TS_XM_LOW; /* XM = 0 */

    /* pullup resistor on */
    TS_YP_PULLUP_UP;
}



void init_ts(void)
{
    /////////////////////////
    // SET UP GPIO AND ADC //
    /////////////////////////
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // YP
    GPIO_InitStruct.GPIO_Pin = TS_YP_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TS_YP_PORT, &GPIO_InitStruct);

    // YM
    GPIO_InitStruct.GPIO_Pin = TS_YM_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TS_YM_PORT, &GPIO_InitStruct);

    // XP
    GPIO_InitStruct.GPIO_Pin = TS_XP_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TS_XP_PORT, &GPIO_InitStruct);

    // XM
    GPIO_InitStruct.GPIO_Pin = TS_XM_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TS_XM_PORT, &GPIO_InitStruct);


    ADC_InitTypeDef  ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);
    ADC_Init(ADC1, &ADC_InitStruct);

    ////////////////////////////////
    // FOR DEBOUNCE AND INTERRUPT //
    ////////////////////////////////

    ts_config_pin_ts(); //set pins for input read
    setup_debounce_timer();
    init_ts_interrupt(); //exti peripheral setup
    enable_debounce_timer(); // Kick the timer off once so that it settles before used (so initial press isn't used twice)
}



uint8_t TS_IO_DetectTouch(void)
{
    uint8_t ret;

    TS_YP_MODE_INPUT; /* YP = D_INPUT */
    TS_YM_MODE_INPUT; /* YM = D_INPUT */
    TS_XP_MODE_OUTPUT;
    TS_XM_MODE_OUTPUT;
    TS_XP_LOW; /* XP = 0 */
    TS_XM_LOW; /* XM = 0 */             

    /* pullup resistor on */
    TS_YP_PULLUP_UP;
    // GPIOX_PUPDR(MODE_PU_UP, TS_YP);

    micro_wait(100);

    if (TS_YP_IDR) {
        ret = 0; /* Touchscreen is not touch */
    } else {
        ret = 1; /* Touchscreen is touch */
    }                        

    /* pullup resistor off */
    TS_YP_PULLUP_OFF;
    // GPIOX_PUPDR(MODE_PU_NONE, TS_YP);

    TS_XP_HIGH;                           /* XP = 1 */
    TS_XM_HIGH;                           /* XM = 1 */
    TS_YP_MODE_OUTPUT;                    /* YP = OUT */
    TS_YM_MODE_OUTPUT;                    /* YM = OUT */

    return ret;
}



//-----------------------------------------------------------------------------
/* read the X position */
uint16_t TS_IO_GetX(void)
{
    uint16_t adc_val;
    uint16_t ret;

    ADC_Cmd(ADC1, DISABLE);
    ADC_RegularChannelConfig(ADC1, TS_YP_ADC_CHANNEL, 1, ADC_SampleTime_112Cycles);
    ADC_Cmd(ADC1, ENABLE);

    TS_YM_MODE_INPUT; /* YM = D_INPUT */
    TS_YP_MODE_ANALOG; /* YP = AN_INPUT */

    TS_XM_MODE_OUTPUT;
    TS_XP_MODE_OUTPUT;
    TS_XM_HIGH; /* XM = 1 */ 
    TS_XP_LOW; /* XP = 0 */

    // ADC Conversion
    ADC_SoftwareStartConv(ADC1);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    adc_val = ADC_GetConversionValue(ADC1);

    TS_XP_HIGH; /* XP = 1 */
    TS_YP_MODE_OUTPUT;
    TS_YM_MODE_OUTPUT;

    // Convert voltage reading to coordinates
    adc_val = (adc_val > X_VOLTAGE_MAX) ? X_VOLTAGE_MAX : adc_val;
    adc_val = (adc_val < X_VOLTAGE_MIN) ? X_VOLTAGE_MIN : adc_val;
    adc_val = adc_val - X_VOLTAGE_MIN;
    ret = (adc_val * X_SIZE) / (X_VOLTAGE_MAX - X_VOLTAGE_MIN);

    if (FLIP_X_COORD) {
        ret = X_SIZE - ret;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/* read the Y position */
uint16_t TS_IO_GetY(void)
{
    uint16_t adc_val;
    uint16_t ret;

    ADC_Cmd(ADC1, DISABLE);
    ADC_RegularChannelConfig(ADC1, TS_XM_ADC_CHANNEL, 1, ADC_SampleTime_112Cycles);
    ADC_Cmd(ADC1, ENABLE);

    TS_XP_MODE_INPUT; /* YM = D_INPUT */
    TS_XM_MODE_ANALOG; /* YP = AN_INPUT */

    TS_YM_MODE_OUTPUT;
    TS_YP_MODE_OUTPUT;
    TS_YM_LOW; /* YM = 0 */
    TS_YP_HIGH; /* YP = 1 */

    // ADC Conversion
    ADC_SoftwareStartConv(ADC1);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    adc_val = ADC_GetConversionValue(ADC1);

    TS_YM_HIGH; /* YM = 1 */
    TS_XP_MODE_OUTPUT;
    TS_XM_MODE_OUTPUT;

    // Convert voltage reading to coordinates
    adc_val = (adc_val > Y_VOLTAGE_MAX) ? Y_VOLTAGE_MAX : adc_val;
    adc_val = (adc_val < Y_VOLTAGE_MIN) ? Y_VOLTAGE_MIN : adc_val;
    adc_val = adc_val - Y_VOLTAGE_MIN;
    ret = (adc_val * Y_SIZE) / (Y_VOLTAGE_MAX - Y_VOLTAGE_MIN);

    if (FLIP_Y_COORD) {
        ret = Y_SIZE - ret;
    }

    return ret;
}
