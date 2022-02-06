/*******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
*******************************************************************************/

#include "stm32f4xx.h"
#define ONE_MILLION 1000000

void config_pa0(void);
static inline void set_pa0(void);
static inline void clear_pa0(void);
void config_tim5(void);

int main(void) {

	config_pa0();
	config_tim5();

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
void config_tim5(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

	TIM5->CR1 = 0; /* set to defaults and disable counting behavior */
  
	/* Configure the PSC and the ARR to make ISR fire rate = 0.25 seconds */
	TIM5->PSC = 1000 - 1;
	TIM5->ARR = 1000 - 1;

	TIM5->DIER |= TIM_DIER_UIE; /* enable update event ISR */

	/* Enable the ISR in the NVIC */
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_SetPriority(TIM5_IRQn, 0);

	/* let the timer begin counting */
	TIM5->CR1 |= TIM_CR1_CEN;
}

static uint16_t pa0_output_status = 0x0;

/* Override the weak declaration in startup.s: toggle PA0 output */
void TIM5_IRQHandler(void) {
	TIM5->SR &= ~TIM_SR_UIF; /* acknowledge the ISR */

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
