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

#define TEST_HX711 1

//static char line1[MAX_CHARS_PER_LINE];

#if TEST_HX711
static char line2[MAX_CHARS_PER_LINE];
extern HX711_Data_t adc;				/* contains samples and moving average! */
#endif /* TEST_HX711 */

#define ONE_MILLION 1000000

	/* TODO: replace SystemInit() in startup_stm32f4xx.c */
int main(void) {
	
	/* Configure the PA5 Nucleo Board LED pin for output */
	config_pa5_nucleo_ld2();

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
}
