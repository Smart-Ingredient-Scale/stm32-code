#include "stm32f4xx.h"

/* Function declarations */
void setup_spi1();
void spi_send(uint16_t n);
void test_ss_display();
void off_display();
void init_display();
void ss_display_num(int32_t n);
