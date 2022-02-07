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

	/* TODO: replace SystemInit() in startup_stm32f4xx.c */
int main(void) {
	/* Configure the PA5 Nucleo Board LED pin for output */
	config_pa5_nucleo_ld2();

	// load_cell_init(); /* configure TIM4 CH4 and other peripherals */

	for(;;) {
#if 1
    set_pa5(); /* toggle the eval board green LED */
    micro_wait(1000);
    clear_pa5();
    micro_wait(1000);
#endif
	}
}
