#include "stm32f4xx.h"
#include "gpio.h"

 /* configure PA5 for output (debugging) */
void config_pa5_nucleo_ld2(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* configure PA5 for GP output */
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
};

void set_pa5(void) {
    GPIOA->BSRR |= GPIO_BSRR_BS_5;
}

void clear_pa5(void) {
    GPIOA->BSRR |= GPIO_BSRR_BR_5;
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

/* Set PA0 high */
void set_pa0(void) {
	GPIOA->BSRR |= GPIO_BSRR_BS_0;
}

/* Force PA0 low */
void clear_pa0(void) {
	GPIOA->BSRR |= GPIO_BSRR_BR_0;
}
