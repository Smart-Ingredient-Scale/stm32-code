#include "stm32f4xx.h"
#include "gpio.h"

 /* configure PA5 for output (debugging) */
void config_pa5_nucleo_ld2(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* configure PA5 for GP output */
    GPIOA->MODER &= ~(GPIO_MODER_MODER5);
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5);
};

void set_pa5(void) {
    GPIOA->BSRR |= GPIO_BSRR_BS_5;
}

void clear_pa5(void) {
    GPIOA->BSRR |= GPIO_BSRR_BR_5;
}