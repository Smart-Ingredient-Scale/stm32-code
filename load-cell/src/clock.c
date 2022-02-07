#include "stm32f4xx.h"
#include "clock.h"

/* ==================== SYSTEM CLOCK CONFIG ======================= */

/* Configure the PLL to get an AHB clock of 64MHz,
 * APB1 clock of 32 MHz, and APB2 clock of 64 MHz.
 * All clocks derived from HSI (16 MHz) internal oscillator. */
void config_system_clocks(void) {
    /* make sure the internal oscillator is on AND ready */
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

    /* Make sure the 16MHz clock is being used as the system clock,
     * and also configure the AHB and APB2 prescalers to be 1 
     * while the APB1 prescaler is set to 2 */
    volatile uint32_t desired_rcc_cfgr = 0x0;

    desired_rcc_cfgr |= (RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_0); /* make sure APB1 prescaler
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
    RCC->CFGR |= RCC_CFGR_SW_1;     /* PLL output becomes SYSCLK */

    /* wait for system clock (just activated from PLL) to settle by polling status */
    while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
}

/* ==================== MICRO_WAIT ======================= */

/* Configure the SysTick internal countdown timer to activate an ISR
 * upon counter underflow. Make an ISR count the number of these 
 * overflow events, and set a flag when done while CPU polls flag */
void micro_wait(uint32_t microsDelay) {

    uint32_t ahbClocksToDelay = microsDelay * AHB_CLKS_PER_US;
    uint32_t maxSystickOverflows = ahbClocksToDelay >> SYSTICK_COUNTDOWN_BITS;
    uint32_t residualClocks = ahbClocksToDelay - (maxSystickOverflows << SYSTICK_COUNTDOWN_BITS);

    /* waste clocks for amount of maximum Systick delays in the microsDelay */
    while(maxSystickOverflows) {
        activate_systick_no_isr(SYSTICK_COUNTDOWN_MAX);
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
        maxSystickOverflows--;
    }

    /* waste the rest of the clocks */
    if(residualClocks != 0) {
        activate_systick_no_isr(residualClocks);
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }        

    return;
}

/* Configure the SysTick timer to operate directly from the 64MHz
 * AHB bus clock (HCLK). activate its ISR after `numClocks`. 
 * `numClocks` can only be 24-bit value. */
void activate_systick_no_isr(uint32_t numAhbClocks) {
    /* ==== See page 251 of STM32F4xx programming manual ==== */
    numAhbClocks &= SYSTICK_COUNTDOWN_MAX; /* only keep the 24 LSBs */

    SysTick->LOAD = numAhbClocks;           /* counter start value */
    SysTick->VAL |= SYSTICK_COUNTDOWN_MAX;  /* clear the counter */
    SysTick->CTRL = 0x5;                    /* use AHB clock and begin counting */
}
