#include "stm32f4xx.h"
#include "clock.h"

/* Reconfigure AHB, APB1, and APB2 clocks */
void config_pc9_sysclk(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    /* MCU output from SYSCLK on PC9 */
    GPIOC->MODER &= ~(GPIO_MODER_MODER9);

    /* allow higher clock output speed */
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11;

    /* reconfig the system clock to be the HSI oscillator (16 MHz) */
    RCC->CR |= RCC_CR_HSION;

    while(!(RCC->CR & RCC_CR_HSIRDY)); /* wait for HSI to stabilize */

    /* remux the system clock to the HSI clock */
    RCC->CFGR &= ~(RCC_CFGR_SWS | RCC_CFGR_SW);
}

/* Configure the PLL to get an AHB clock of 64MHz,
 * APB1 clock of 32 MHz, and APB2 clock of 64 MHz.
 * All clocks derived from HSI (16 MHz) internal oscillator. */
void config_pll_from_hsi(void) {
    /* make sure the internal oscillator is on AND ready */
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    /* Make sure the 16MHz clock is being used as the system clock,
     * and also configure the AHB and APB2 prescalers to be 1 
     * while the APB1 prescaler is set to 2 */
    volatile uint32_t desired_rcc_cfgr = 0x0;

    desired_rcc_cfgr |= (RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_0); /* make sure APB2 prescaler
                                                                * is 2: datasheet wrong!!! */

    RCC->CFGR = desired_rcc_cfgr;

    /* Now, the 16MHz clock has been muxed to the system clock...
     * it is now time to deactivate the PLL clock (if activated) */
    RCC->CR &= ~(RCC_CR_PLLON);
    while(RCC->CR & RCC_CR_PLLRDY);

    /* It is now time to deactivate the HSE clock (if activated) */
    RCC->CR &= ~(RCC_CR_HSEON);
    while(RCC->CR & RCC_CR_HSERDY);

    /* Now time to configure the 16MHz HSI clock to be muxed as input
     * to the PLL (instead of the possible HSE external clock) */
    volatile uint32_t rcc_pllcfgr_reg = RCC->PLLCFGR;
    rcc_pllcfgr_reg &= ~(RCC_PLLCFGR_PLLM |
                         RCC_PLLCFGR_PLLN |
                         RCC_PLLCFGR_PLLP |
                         RCC_PLLCFGR_PLLQ |
                         RCC_PLLCFGR_PLLSRC); /* set for HSI input */

    rcc_pllcfgr_reg |= ((32 << 0)   |   /* PLLM = 32 */
                        (256 << 6)  |   /* PLLN = 256 */
                        (0 << 16)   |   /* PLLP = 2 */    
                        (4 << 24));     /* PLLQ = 4 */

    RCC->PLLCFGR = rcc_pllcfgr_reg;

    /* time to activate the PLL output clock and wait to stabilize */
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* make sure the RCC is configured for deriving the system clock
     * from the output of the PLL */
    desired_rcc_cfgr = RCC->CFGR;
    desired_rcc_cfgr &= ~(RCC_CFGR_SW |
                          RCC_CFGR_SWS);
    
    desired_rcc_cfgr |= (RCC_CFGR_SWS_1 | /* PLL output becomes SYSCLK */
                         RCC_CFGR_SW_1);

    RCC->CFGR = desired_rcc_cfgr;
}
