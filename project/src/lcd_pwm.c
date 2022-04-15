#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "lcd_pwm.h"

// PWM on PC8

void LCD_PWM_TIM_Init(void) {
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	
	/* Enable clock for TIM3 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_BaseStruct.TIM_Prescaler = LCD_PWM_PRESCALAR;

	/* Count up */
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_BaseStruct.TIM_Period = LCD_PWM_PERIOD; 
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;

	/* Initialize TIM3 */
    TIM_TimeBaseInit(TIM3, &TIM_BaseStruct);

	/* Start count on TIM3 */
    TIM_Cmd(TIM3, ENABLE);
}

void LCD_PWM_Init(uint8_t duty_cycle) {
	TIM_OCInitTypeDef TIM_OCStruct;
	
	/* PWM mode 2 = Clear on compare match */
	/* PWM mode 1 = Set on compare match */
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	// TIM_OCStruct.TIM_Pulse = 2099; /* 50% duty cycle */
	TIM_OCStruct.TIM_Pulse = ((LCD_PWM_PERIOD + 1) * duty_cycle) / 100 - 1;
	TIM_OC3Init(TIM3, &TIM_OCStruct);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

void LCD_PWM_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Clock for GPIOD */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Alternating functions for pins */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

	/* Set pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}