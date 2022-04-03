 #ifndef CLOCK_H
 #define CLOCK_H

 #define RYAN_CLOCK_EDITS

 /* Clocks are defined based on call to config_pll_from_hsi() */
 #define HSI_CLK     16000000U /* 16 MHz HSI internal clock */
 #define AHB_CLK     64000000U /* 64 MHz AHB clock from PLL output */
 #define CPU_CLK     AHB_CLK   /* another name for the same thing */
 #define APB1_CLK    32000000U /* 32 MHz derived from PLL output / 2 */
 #define APB2_CLK    64000000U /* 64 MHz APB2 clock from PLL output */

 /* define some useful peripherals' bus clocks: see pg. 12 of datasheet */
 #define GPIOA_CLK   AHB_CLK
 #define GPIOB_CLK   AHB_CLK
 #define GPIOC_CLK   AHB_CLK
 #define GPIOD_CLK   AHB_CLK
 #define GPIOE_CLK   AHB_CLK
 #define GPIOF_CLK   AHB_CLK

 #define SPI1_CLK    APB2_CLK
 #define SPI2_CLK    APB1_CLK
 #define SPI3_CLK    APB1_CLK
 #define SPI4_CLK    APB2_CLK

 #define TIM1_CLK    APB2_CLK
 #define TIM2_CLK    APB1_CLK
 #define TIM3_CLK    APB1_CLK
 #define TIM4_CLK    APB1_CLK
 #define TIM5_CLK    APB1_CLK
 #define TIM9_CLK    APB2_CLK
 #define TIM10_CLK   APB2_CLK
 #define TIM11_CLK   APB2_CLK

 #define DMA1_CLK	AHB_CLK
 #define DMA2_CLK	AHB_CLK

 #define I2C1_CLK	APB1_CLK
 #define I2C2_CLK	APB1_CLK
 #define I2C3_CLK	APB1_CLK

 #define ADC1_CLK	APB2_CLK

 #define USART1_CLK	APB2_CLK
 #define USART6_CLK	APB2_CLK
 #define USART2_CLK	APB1_CLK

 #define WWDG_CLK	APB1_CLK
 #define SDIO_CLK	APB2_CLK
 #define EXTI_CLK	APB2_CLK

 /* constant for the SysTick micro_wait() timer routine */
 #define SYSTICK_COUNTDOWN_MAX   0xFFFFFFU
 #define SYSTICK_COUNTDOWN_BITS   24
 #define US_PER_SEC              1000000U
 #define AHB_CLKS_PER_US         (AHB_CLK / US_PER_SEC)

 /* function declarations */
 void config_system_clocks(void);
 void micro_wait(uint32_t microsDelay);
 void activate_systick_no_isr(uint32_t numAhbClocks);

 #endif /* CLOCK_H */
