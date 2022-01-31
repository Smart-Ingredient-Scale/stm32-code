/******************************************************************************
 * File:    	main.c
 * Author  		Ryan Montsma
 * @date    	Jan 31, 2022
 * Description: Test MCU code to blink an LED on PA0 using a timer's ISR.
******************************************************************************/

#include "stm32f4xx.h"
#define ONE_MILLION 1000000

void config_pa0(void);
static inline void set_pa0(void);
static inline void clear_pa0(void);
static inline void enable_tim_clock(TIM_TypeDef *TIMx);
void config_tim_uev(TIM_TypeDef *TIMx, enum IRQn IRQnum);

int main(void) {

	config_pa0();

	/* try both of these function calls */
	config_tim_uev(TIM2, TIM2_IRQn);
//	config_tim_uev(TIM5, TIM5_IRQn);

	for(;;) {
#if 0
    set_pa0();
    for(uint32_t i = 0; i < ONE_MILLION*3; i++);
    clear_pa0();
    for(uint32_t i = 0; i < ONE_MILLION*3; i++);
#endif
	}
}

/* Configure PA0 GPIO pin for push-pull output */
void config_pa0(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	/* configure the GPIO for output mode */
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	GPIOA->MODER |= GPIO_MODER_MODER0_0;

	GPIOA->OTYPER &= ~GPIO_OTYPER_IDR_0; /* push-pull output */

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0; /* no pull up/down down resistor */

	GPIOA->BSRR |= GPIO_BSRR_BR_0; /* reset PA0 (low voltage) */
}

/* Configure a timer for an update event ISR to fire upon a counter overflow
 * or underflow event. */
void config_tim_uev(TIM_TypeDef *TIMx, enum IRQn irqn) {

	/* enable the APB clock to the peripheral using the RCC */
	enable_tim_clock(TIMx); 

	TIMx->CR1 = 0; /* set to defaults and disable counting behavior */

	/* Configure the PSC and the ARR to make ISR fire rate = APB_CLK/1,000,000 seconds */
	TIMx->PSC = 10000 - 1;
	TIMx->ARR = 1000 - 1;

	TIMx->DIER |= TIM_DIER_UIE; /* enable update event ISR */

	/* Enable the ISR in the NVIC */
	NVIC_EnableIRQ(irqn);
	NVIC_SetPriority(irqn, 0);

	/* let the timer begin counting */
	TIMx->CR1 |= TIM_CR1_CEN;
}

static uint16_t pa0_output_status = 0x0;

/* Override the weak declaration in startup.s: toggle PA0 output */
void TIM5_IRQHandler(void) {
	TIM5->SR &= ~TIM_SR_UIF; /* acknowledge the ISR */

	GPIOA->BSRR |= GPIO_BSRR_BR_0;
	GPIOA->BSRR |= (pa0_output_status);

	pa0_output_status ^= 0x1;
}

/* Override the weak declaration in startup.s: toggle PA0 output */
void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM_SR_UIF; /* acknowledge the ISR */

	GPIOA->BSRR |= GPIO_BSRR_BR_0;
	GPIOA->BSRR |= (pa0_output_status);

	pa0_output_status ^= 0x1;
}

static inline void set_pa0(void) {
	GPIOA->BSRR |= GPIO_BSRR_BS_0;
}

static inline void clear_pa0(void) {
	GPIOA->BSRR |= GPIO_BSRR_BR_0;
}

static inline void enable_tim_clock(TIM_TypeDef *TIMx) {
	switch((uint32_t)TIMx) {
		case (uint32_t) TIM2:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			break;
		case (uint32_t) TIM3:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			break;
		case (uint32_t) TIM4:
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			break;
		case (uint32_t) TIM5:
			RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
			break;
		case (uint32_t) TIM6: /* N/A */
			RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
			break;
		case (uint32_t) TIM7: /* N/A */
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
			break;
		case (uint32_t) TIM12: /* N/A? */
			RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
			break;
		case (uint32_t) TIM13: /* N/A? */
			RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
			break;
		
		case (uint32_t) TIM9:
			RCC->APB1ENR |= RCC_APB2ENR_TIM9EN;
			break;
		case (uint32_t) TIM10:
			RCC->APB1ENR |= RCC_APB2ENR_TIM10EN;
			break;
		case (uint32_t) TIM11:
			RCC->APB1ENR |= RCC_APB2ENR_TIM11EN;
			break;
	}
}
