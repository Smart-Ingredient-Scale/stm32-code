/*******************************************************************************
 * @file    main.c
 * @author  Ryan Montsma
 * @version V1.0
 * @date    Feb 18, 2022
 * @brief   Contains code in load-cell.c that configures TIM4 to handle
 *          the PWMing of the SDCLK line to the HX711 load cell. The ISR
 *          samples the SDATA line. The SDCLK is on PB9 (TIM4_CH4). The
 *          SDATA is on PB8 (generic GPIO). The SPI OLED SOC1602A will
 *          display the integer value of the moving average of the past
 *          16 samples (configurable in load-cell.h).
*******************************************************************************/

#include "stm32f4xx.h"
#include "clock.h"
#include "gpio.h"
#include "load-cell.h"
#include <stdio.h>
#include "spi-ss.h"

extern HX711_Data_t adc;                /* contains samples and moving average! */

#define ONE_MILLION 1000000

int main(void) {
    /* configure the SPI OLED GPIO pins:
     * PC10 --> SCK  --> OLED pin 12,
     * PC11 --> NSS  --> OLED pin 16,
     * PC12 --> MOSI --> OLED pin 14 */
    //config_oled_gpio_bb();

    // Seven-seg display initialzation
    setup_spi1();
    off_display();
    init_display();


    init_button();
    init_button_interrupt();


    load_cell_init(HZ10, CHA_128_GAIN); /* configure TIM4 CH4 and other peripherals */

    /* actually activate the load cell sampling timer */
    load_cell_enable();

    for(;;) {
        int32_t converted = convert(adc.movingAverage);
        //sprintf(line2, "%+ld", converted);
        //print_oled_bb("ADC Average:", line2);
        ss_display_num(converted);
    }
}
