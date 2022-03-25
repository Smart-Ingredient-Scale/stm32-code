/*******************************************************************************
 * @file    main.c
 * @author  Ryan Montsma
 * @version V1.0
 * @date    Feb 18, 2022
 * @brief   Contains code in load-cell.c that configures TIM4 to handle
 * 			the PWMing of the SDCLK line to the HX711 load cell. The ISR
 * 			samples the SDATA line. The SDCLK is on PB9 (TIM4_CH4). The
 * 			SDATA is on PB8 (generic GPIO). The SPI OLED SOC1602A will
 * 			display the integer value of the moving average of the past
 * 			16 samples (configurable in load-cell.h).
*******************************************************************************/

#include "stm32f4xx.h"
#include "clock.h"
#include "gpio.h"
#include "load-cell.h"
#include "spi-oled.h"
#include <stdio.h>

//#define CLOCK_TEST
#define TEST_HX711 1

//static char line1[MAX_CHARS_PER_LINE];

#if TEST_HX711 && !defined(CLOCK_TEST)
static char line2[MAX_CHARS_PER_LINE];
extern HX711_Data_t adc;				/* contains samples and moving average! */
#endif /* TEST_HX711 */

#ifdef CLOCK_TEST
static inline void enable_tim_clock(TIM_TypeDef *TIMx);
void config_tim_uev(TIM_TypeDef *TIMx, enum IRQn IRQnum);

#endif /* CLOCK_TEST */

#define ONE_MILLION 1000000

	/* TODO: replace SystemInit() in startup_stm32f4xx.c */
int main(void) {
	
#ifdef CLOCK_TEST
	/* Configure the PA5 Nucleo Board LED pin for output */
	config_pa5_nucleo_ld2();
//	config_tim_uev(TIM1, TIM1_UP_TIM10_IRQn);

	for(;;) {
		micro_wait(1);
		set_pa5();
		micro_wait(1);
		clear_pa5();
	}

#endif /* CLOCK_TEST */

#ifndef CLOCK_TEST
	/* configure the SPI OLED GPIO pins:
	 * PC10 --> SCK  --> OLED pin 12,
	 * PC11 --> NSS  --> OLED pin 16,
	 * PC12 --> MOSI --> OLED pin 14 */
	config_oled_gpio_bb();

	/* send the OLED initialization sequence */
	init_oled_bb();
	print_oled_bb("HX711 Init:", "In Progress");

	load_cell_init(HZ10, CHA_128_GAIN); /* configure TIM4 CH4 and other peripherals */
	print_oled_bb("HX711 Init done", "SDA-PB8 CLK-PB9");

	/* wait 2 seconds for message to show */
	micro_wait(2 * ONE_MILLION);

	/* actually activate the load cell sampling timer */
	load_cell_enable();

	for(;;) {
#if TEST_HX711
		sprintf(line2, "%+ld", adc.movingAverage);
		print_oled_bb("ADC Average:", line2);
#endif /* TEST_HX711 */
	}
#endif /* CLK_TEST */
}

#ifdef CLOCK_TEST

volatile uint8_t pa5Status = 0;
void TIM1_UP_TIM10_IRQHandler(void) {
	TIM1->SR &= ~TIM_SR_UIF;

	if(pa5Status == 1) {
		clear_pa5();
		pa5Status = 0x0;
	}

	else {
		set_pa5();
		pa5Status = 0x1;
	}

}

/* Configure a timer for an update event ISR to fire upon a counter overflow
 * or underflow event. */
void config_tim_uev(TIM_TypeDef *TIMx, enum IRQn irqn) {

	/* enable the APB clock to the peripheral using the RCC */
	enable_tim_clock(TIMx);

	TIMx->CR1 = 0; /* set to defaults and disable counting behavior */

	/* Configure the PSC and the ARR to make ISR fire rate = APB_CLK/1,000,000 seconds */
	TIMx->PSC = (TIM1_CLK / US_PER_SEC) - 1;
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

static inline void enable_tim_clock(TIM_TypeDef *TIMx) {
	switch((uint32_t)TIMx) {
		case ((uint32_t) TIM1):
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			break;
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

/* void test_ahb2_clock_500Hz_PA5 */

#endif /* CLOCK_TEST */
