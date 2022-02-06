/*******************************************************************************
 * @file    main.c
 * @author  Ryan Montsma
 * @version V1.0
 * @date    Feb 5, 2022
 * @brief   Contains skeleton code that sets up the ISR for sampling the
 * 			load cell and the PWM hardware for the HX711 clock. Needs
 * 			to have functions to disable the PWM and to read from the 
 * 			PB8 digital input pin.
*******************************************************************************/

#include "stm32f4xx.h"
#include "clock.h"
#include "gpio.h"
#include "load-cell.h"

#define ONE_MILLION 1000000

int main(void) {
	config_pll_from_hsi(); 	/* TODO: replace SystemInit() in startup_stm32f4xx.c */
	
	/* Conifugre the PA5 Nucleo Board LED pin for output */
	config_pa5_nucleo_ld2();

	load_cell_init(); /* configure TIM4 CH4 and other peripherals */

	for(;;) {
#if 0
    set_pa0();
    for(uint32_t i = 0; i < ONE_MILLION*3; i++);
    clear_pa0();
    for(uint32_t i = 0; i < ONE_MILLION*3; i++);
#endif
	}
}