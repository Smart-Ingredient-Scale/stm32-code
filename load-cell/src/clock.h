#define HSI_CLK     16000000U /* 16 MHz HSI internal clock */

/* Clocks are defined based on call to config_pll_from_hsi() */
#define AHB_CLK     64000000U /* 64 MHz AHB clock from PLL output */
#define CPU_CLK     AHB_CLK   /* another name for the same thing */
#define APB1_CLK    32000000U /* 32 MHz derived from PLL output / 2 */
#define APB2_CLK    64000000U /* 64 MHz APB2 clock from PLL output */

/* define some useful peripherals' bus clocks */
#define GPIOA_CLK   AHB_CLK
#define GPIOB_CLK   AHB_CLK
#define GPIOC_CLK   AHB_CLK
#define GPIOD_CLK   AHB_CLK
#define GPIOE_CLK   AHB_CLK
#define GPIOF_CLK   AHB_CLK
#define SPI1_CLK    AHB_CLK
#define SPI2_CLK    APB1_CLK
#define SPI3_CLK    APB1_CLK
#define SPI4_CLK    AHB_CLK
#define TIM1_CLK    APB2_CLK
#define TIM2_CLK    APB1_CLK
#define TIM3_CLK    APB1_CLK    
#define TIM4_CLK    APB1_CLK
#define TIM5_CLK    APB1_CLK
#define TIM9_CLK    APB2_CLK
#define TIM10_CLK   APB2_CLK
#define TIM11_CLK   APB2_CLK

/* constant for the SysTick micro_wait() timer routine */
#define SYSTICK_COUNTDOWN_MAX   0xFFFFFFU
#define SYSTICK_COUNTDOWN_BITS   24
#define US_PER_SEC              1000000U
#define AHB_CLKS_PER_US         (AHB_CLK / US_PER_SEC)

/* function declarations */
void config_system_clocks(void);
void micro_wait(uint32_t microsDelay);
void activate_systick_no_isr(uint32_t numAhbClocks);
