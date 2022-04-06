/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
*/


#include "stm32f4xx.h"

#include "stm32_adafruit_lcd.h"
#include "ili9341.h"
#include "lcd_io_spi.h"

#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_tim.h"
#include "touchscreen.h"

#include "screen.h"

#include "clock.h"
#include "gpio.h"
#include "load-cell.h"
#include <stdio.h>
#include "spi-ss.h"

// touchscreen.c
extern uint16_t ts_xpos;
extern uint16_t ts_ypos;
extern int      ts_pressed;

// screen.c
extern struct Screen home_screen;
extern struct Screen information_screen;
extern struct Screen vol_cal_screen;
extern struct Screen mass_cal_screen;
extern HX711_Data_t adc;                /* contains samples and moving average! */

#define ONE_MILLION 1000000


int main(void)
{
    // Seven-seg display initialzation
    setup_spi1();
    off_display();
    init_display();

    init_button();
    init_button_interrupt();

    load_cell_init(HZ10, CHA_128_GAIN); /* configure TIM4 CH4 and other peripherals */

    /* actually activate the load cell sampling timer */
    load_cell_enable();


    // Ben's code
    BSP_LCD_Init();
    //init_ts();

    struct Screen *cur_screen = &home_screen;
    draw_home_screen("test10", "test2", "test3");


    uint16_t x_pos;
    uint16_t y_pos;

    while(1) {
        /*
        // Event processing
        if(ts_pressed) {
            x_pos = ts_xpos;
            y_pos = ts_ypos;
            ts_pressed = 0;

            // Debug for location info on screen press
            // char temp_str[25];
            // sprintf(temp_str, "%d %d", x_pos, y_pos);
            // BSP_LCD_DisplayStringAt(60, 230, (uint8_t *)temp_str, CENTER_MODE);

            struct Button cur_button;

            for (uint8_t i = 0; i < cur_screen->num_buttons; i++) {
                cur_button = cur_screen->buttons[i];
                if (inBox(x_pos, y_pos, cur_button.x0, cur_button.y0, cur_button.x1, cur_button.y1)) {
                    // char str[10];
                    // sprintf(str, "%d", cur_button.process_id);
                    // BSP_LCD_DisplayStringAt(42, 270, (uint8_t *)str, CENTER_MODE);
                    process_button(&cur_screen, cur_button.process_id);
            
                    break;
                }
            }
        }
*/
        // Display the current load in g
        int32_t converted = convert(adc.movingAverage);
        ss_display_num(converted);


    }


    for(;;);
}
