#ifndef _LCD_PWM_H_
#define _LCD_PWM_H_
#include "stm32f4xx.h"


// PC8

void LCD_PWM_GPIO_Init(void);
void LCD_PWM_Init(uint8_t duty_cycle);
void LCD_PWM_TIM_Init(void);

/* 10kHz PWM Example */
// #define LCD_PWM_PRESCALAR               0
// #define LCD_PWM_PERIOD                  8399

#define LCD_PWM_PRESCALAR               0
#define LCD_PWM_PERIOD                  8399

#endif //_LCD_PWM_H_
