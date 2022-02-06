#define HSI_CLK     16000000U /* 16 MHz HSI internal clock */

/* Clocks are defined based on call to config_pll_from_hsi() */
#define AHB_CLK     64000000U /* 64 MHz AHB clock from PLL output */
#define CPU_CLK     AHB_CLK   /* another name for the same thing */
#define APB1_CLK    32000000U /* 32 MHz derived from PLL output / 2 */
#define APB2_CLK    64000000U /* 64 MHz APB2 clock from PLL output */

/* function declarations */
void config_pc9_sysclk(void);
void config_pll_from_hsi(void);
