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

int main(void) {
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
        ss_display_num(converted);
    }
}
