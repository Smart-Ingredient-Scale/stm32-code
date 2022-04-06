#include "stm32f4xx.h"

// External functions
void     init_ts(void);
int      inBox(uint16_t x, uint16_t y, uint16_t x_low, uint16_t y_low, uint16_t x_high, uint16_t y_high);
uint8_t  TS_IO_DetectTouch(void);
uint16_t TS_IO_GetX(void);
uint16_t TS_IO_GetY(void);



// Voltage to Point Translation
#define X_VOLTAGE_MAX 3700
#define X_VOLTAGE_MIN 400
#define Y_VOLTAGE_MAX 3800
#define Y_VOLTAGE_MIN 600
#define X_SIZE 240
#define Y_SIZE 320

#define FLIP_X_COORD 0
#define FLIP_Y_COORD 1




// YM = PC5
#define TS_YM_PORT                     GPIOC
#define TS_YM_PIN                      GPIO_Pin_5

#define TS_YM_MODE_INPUT    GPIOC -> MODER &= ~GPIO_MODER_MODER5
#define TS_YM_MODE_ANALOG   GPIOC -> MODER |= GPIO_MODER_MODER5
#define TS_YM_MODE_OUTPUT   {GPIOC -> MODER &= ~GPIO_MODER_MODER5; GPIOC -> MODER |= GPIO_MODER_MODER5_0;}
#define TS_YM_LOW           GPIOC -> BSRR |= GPIO_BSRR_BR_5
#define TS_YM_HIGH          GPIOC -> BSRR |= GPIO_BSRR_BS_5

// YP = PB0 (ADC1_IN8)
#define TS_YP_PORT                     GPIOB
#define TS_YP_PIN                      GPIO_Pin_0
#define TS_YP_ADC_CHANNEL              ADC_Channel_8

#define TS_YP_MODE_INPUT    GPIOB -> MODER &= ~GPIO_MODER_MODER0
#define TS_YP_MODE_ANALOG   GPIOB -> MODER |= GPIO_MODER_MODER0
#define TS_YP_MODE_OUTPUT   {GPIOB -> MODER &= ~GPIO_MODER_MODER0; GPIOB -> MODER |= GPIO_MODER_MODER0_0;}
#define TS_YP_LOW           GPIOB -> BSRR |= GPIO_BSRR_BR_0
#define TS_YP_HIGH          GPIOB -> BSRR |= GPIO_BSRR_BS_0

#define TS_YP_PULLUP_UP     {GPIOB -> PUPDR &= ~GPIO_PUPDR_PUPDR0; GPIOB -> PUPDR |= GPIO_PUPDR_PUPDR0_0;}
#define TS_YP_PULLUP_OFF    GPIOB -> PUPDR &= ~GPIO_PUPDR_PUPDR0
#define TS_YP_IDR           ((GPIOB -> IDR & GPIO_IDR_IDR_0) != 0)

#define TS_YP_EXTI_PORT     EXTI_PortSourceGPIOB
#define TS_YP_EXTI_PIN      EXTI_PinSource0
#define TS_YP_EXTI_LINE     EXTI_Line0

// XM = PC4 (ADC1_IN14)
#define TS_XM_PORT                     GPIOC
#define TS_XM_PIN                      GPIO_Pin_4
#define TS_XM_ADC_CHANNEL              ADC_Channel_14

#define TS_XM_MODE_INPUT    GPIOC -> MODER &= ~GPIO_MODER_MODER4
#define TS_XM_MODE_ANALOG   GPIOC -> MODER |= GPIO_MODER_MODER4
#define TS_XM_MODE_OUTPUT   {GPIOC -> MODER &= ~GPIO_MODER_MODER4; GPIOC -> MODER |= GPIO_MODER_MODER4_0;}
#define TS_XM_LOW           GPIOC -> BSRR |= GPIO_BSRR_BR_4
#define TS_XM_HIGH          GPIOC -> BSRR |= GPIO_BSRR_BS_4

// XP = PB1
#define TS_XP_PORT                     GPIOB
#define TS_XP_PIN                      GPIO_Pin_1

#define TS_XP_MODE_INPUT    GPIOB -> MODER &= ~GPIO_MODER_MODER1
#define TS_XP_MODE_ANALOG   GPIOB -> MODER |= GPIO_MODER_MODER1
#define TS_XP_MODE_OUTPUT   {GPIOB -> MODER &= ~GPIO_MODER_MODER1; GPIOB -> MODER |= GPIO_MODER_MODER1_0;}
#define TS_XP_LOW           GPIOB -> BSRR |= GPIO_BSRR_BR_1
#define TS_XP_HIGH          GPIOB -> BSRR |= GPIO_BSRR_BS_1