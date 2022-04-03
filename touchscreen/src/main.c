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

// image.c
//extern uint8_t* pixels;

// touchscreen.c
extern uint16_t ts_xpos;
extern uint16_t ts_ypos;
extern int      ts_pressed;

// screen.c
extern struct Screen home_screen;
extern struct Screen information_screen;
extern struct Screen vol_cal_screen;
extern struct Screen mass_cal_screen;


int main(void)
{
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//     // PB0 - Analog
//     GPIO_InitTypeDef GPIO_InitStruct;
//     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
//     GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//     GPIO_Init(GPIOB, &GPIO_InitStruct);


//     ADC_InitTypeDef  ADC_InitStruct;
//     ADC_StructInit(&ADC_InitStruct);
//     ADC_Init(ADC1, &ADC_InitStruct);

//     // Channel 8
//     ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_112Cycles);

// //    ADC -> CCR |= (3 << 16); // PCLK2 divided by 8

//     micro_wait(10000);
//     ADC_Cmd(ADC1, ENABLE);
//     micro_wait(10000);

//     uint16_t x_pos;
//     while(1) {
//         ADC_SoftwareStartConv(ADC1);
//         while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
//         x_pos = ADC_GetConversionValue(ADC1);
//         for (int i = 0; i < 20 * 1000; i++);
//     }
   
    BSP_LCD_Init();
    // ili9341_Init();
    init_ts();

    // BSP_LCD_Clear(LCD_COLOR_WHITE);

    // BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    // BSP_LCD_SetBackColor(LCD_COLOR_GREEN);

    // BSP_LCD_FillRect(10, 90, 100, 100);

    // BSP_LCD_SetFont(&Font24);
    // BSP_LCD_DisplayStringAt(50, 100, (uint8_t *)"LCD Demo", CENTER_MODE);
    
//      while(1) {
//     BSP_LCD_Init();
//     draw_home_screen("Test1", "Test2", "");
//      micro_wait(3000000);

//     BSP_LCD_Init();
//     draw_information_screen("a", "b", "c");
//      micro_wait(3000000);


    //  BSP_LCD_Init();
    // draw_vol_cal_screen();
    //  micro_wait(3000000);

    //  BSP_LCD_Init();
    // draw_mass_cal_screen();
    //  micro_wait(3000000);
//      }
//    draw_vol_cal_screen();
//     draw_mass_cal_screen();




    ////////// Demo 2 ////////////////////////////////////////

//     panic("panic messages123456789123456789sdhfaklsjhfaslk");

    struct Screen *cur_screen = &home_screen;
    draw_home_screen("test1", "test2", "test3");




    uint16_t x_pos;
    uint16_t y_pos;

    while(1) {




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



        // Update state


        for (int i = 0; i < 200 * 1000; i++);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







    ///////////////////////////////////// Demo
//    ili9341_Init();
//    init_ts();

//    uint16_t x_pos;
//    uint16_t y_pos;

//    uint8_t state = 0;
//    ili9341_FillRect(0, 0, 100, 100, 0xffff); // state 0
// //
// //
//     while(1) {
//         if(ts_pressed) {
//             x_pos = ts_xpos;
//             y_pos = ts_ypos;
//             ts_pressed = 0;
// //         if (TS_IO_DetectTouch()) {
// //             x_pos = TS_IO_GetX();
// //             y_pos = TS_IO_GetY();
//             if (state == 0 && inBox(x_pos, y_pos, 0, 0, 100, 100)) {
//                 state = 1;
//                 ili9341_FillRect(0, 0, 100, 100, 0x0000);
//                 ili9341_FillRect(140, 0, 100, 100, 0xff00);
//             } else if (state == 1 && inBox(x_pos, y_pos, 140, 0, 240, 100)) {
//                 state = 2;
//                 ili9341_FillRect(140, 0, 100, 100, 0x0000);
//                 ili9341_FillRect(140, 220, 100, 100, 0x00ff);
//             } else if (state == 2 && inBox(x_pos, y_pos, 140, 220, 240, 320)) {
//                 state = 3;
//                 ili9341_FillRect(140, 220, 100, 100, 0x0000);
//                 ili9341_FillRect(0, 220, 100, 100, 0x0ff0);
//             } else if (state == 3 && inBox(x_pos, y_pos, 0, 220, 100, 320)) {
//                 state = 0;
//                 ili9341_FillRect(0, 220, 100, 100, 0x0000);
//                 ili9341_FillRect(0, 0, 100, 100, 0xffff);
//             }
//         }
//         for (int i = 0; i < 20 * 1000; i++);
//     }


     ///////////////////////////////////// Demo









// //    ili9341_Init();
// //    setCDline();
// //    setup_spi2();
// //    while (1) {
// //        send_cmd8(0xa1);
// //        for (int i = 0; i < 10 * 1000; i++);
// //    }



//     //!
// //    LCD_IO_Init();
//    // ili9341_SetDisplayWindow(100, 100, 100, 100);
//     //!

//     ili9341_FillRect(0, 0, 100, 100, 0xffff);
//     ili9341_DrawRGBImage(50, 50, 150, 200, pixels);
//     ili9341_FillRect(50, 50, 150, 13, 0x0000);
//     ili9341_FillRect(180, 50, 20, 200, 0x0000);

    for(;;);
}




// overwriting with smaller text
// state machine for keeping track of calibration
// state machine for keeping track of current position in ingredients

// faster screen switching
    // "inverse" screen write, write same screen but use background color
    // parameter in function for original screen write


// macro to get all of the if statements before and after

// pwm backlight
// clean code
// github