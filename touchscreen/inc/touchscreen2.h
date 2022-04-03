// #include "stm32f4xx.h"

// /* ADC sample time (0:3cycles, 1:15c, 2:28c, 3:55c, 4:84c, 5:112c, 6:144c, 7:480cycles) */
// #define  TS_SAMPLETIME        2

// #define TS_AD_DELAY     500

// /* Link function for Touchscreen */
// void     init_ts(void);
// uint8_t  TS_IO_DetectTouch(void);
// uint16_t TS_IO_GetX(void);
// uint16_t TS_IO_GetY(void);
// uint16_t TS_IO_GetZ1(void);
// uint16_t TS_IO_GetZ2(void);



// // YM = PC5
// #define TS_YM_MODE_INPUT    GPIOC -> MODER &= ~GPIO_MODER_MODER5
// #define TS_YM_MODE_ANALOG   GPIOC -> MODER |= GPIO_MODER_MODER5
// #define TS_YM_MODE_OUTPUT   {GPIOC -> MODER &= ~GPIO_MODER_MODER5; GPIOC -> MODER |= GPIO_MODER_MODER5_0;}
// #define TS_YM_LOW           GPIOC -> BSRRH |= GPIO_BSRR_BS_5
// #define TS_YM_HIGH          GPIOC -> BSRRL |= GPIO_BSRR_BS_5

// // YP = PB0 (ADC1_IN8)
// #define TS_YP_MODE_INPUT    GPIOB -> MODER &= ~GPIO_MODER_MODER0
// #define TS_YP_MODE_ANALOG   GPIOB -> MODER |= GPIO_MODER_MODER0
// #define TS_YP_MODE_OUTPUT   {GPIOB -> MODER &= ~GPIO_MODER_MODER0; GPIOB -> MODER |= GPIO_MODER_MODER0_0;}
// #define TS_YP_LOW           GPIOB -> BSRRH |= GPIO_BSRR_BS_0
// #define TS_YP_HIGH          GPIOB -> BSRRL |= GPIO_BSRR_BS_0

// // XM = PC4 (ADC1_IN14)
// #define TS_XM_MODE_INPUT    GPIOC -> MODER &= ~GPIO_MODER_MODER4
// #define TS_XM_MODE_ANALOG   GPIOC -> MODER |= GPIO_MODER_MODER4
// #define TS_XM_MODE_OUTPUT   {GPIOC -> MODER &= ~GPIO_MODER_MODER4; GPIOC -> MODER |= GPIO_MODER_MODER4_0;}
// #define TS_XM_LOW           GPIOC -> BSRRH |= GPIO_BSRR_BS_4
// #define TS_XM_HIGH          GPIOC -> BSRRL |= GPIO_BSRR_BS_4

// // XP = PB1
// #define TS_XP_MODE_INPUT    GPIOB -> MODER &= ~GPIO_MODER_MODER1
// #define TS_XP_MODE_ANALOG   GPIOB -> MODER |= GPIO_MODER_MODER1
// #define TS_XP_MODE_OUTPUT   {GPIOB -> MODER &= ~GPIO_MODER_MODER1; GPIOB -> MODER |= GPIO_MODER_MODER1_0;}
// #define TS_XP_LOW           GPIOB -> BSRRH |= GPIO_BSRR_BS_1
// #define TS_XP_HIGH          GPIOB -> BSRRL |= GPIO_BSRR_BS_1