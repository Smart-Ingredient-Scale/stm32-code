#ifndef SPI_OLED_H
#define SPI_OLED_H

#define MAX_CHARS_PER_LINE  16  /* maximum of 16 characters a line on OLED */

#define NSS_PIN_NUMBER      4   /* PA4 */
#define SCK_PIN_NUMBER      10  /* PC10 */
#define MOSI_PIN_NUMBER     12  /* PC12 */
#define BITS_PER_AF_PIN     4   /* each pin has 4 bits for alternate function */

/* See STM32F0 family reference manual pg. 790 for correct values of BR */
#define SPI_SCK_PRESCALER   128U
#define SPI_SCK_BR          0x5
#define BAUDS_PER_TRANSMISSION  10
#ifndef US_PER_SEC
#define US_PER_SEC              1000000
#endif /* US_PER_SEC */
#define US_PER_BAUD             (US_PER_SEC / SPI_SCK_SPEED)

#define SPI_SCK_SPEED   (SPI3_CLK / SPI_SCK_PRESCALER)

/* Function Declarations */
#if 0

void config_spi_oled(SPI_TypeDef *SPIx);
void send_oled_cmd8(SPI_TypeDef *SPIx, uint8_t commandByte);
void spi_init_oled(SPI_TypeDef *SPIx);
void send_oled_line1(SPI_TypeDef *SPIx, const char *line1);
void send_oled_line2(SPI_TypeDef *SPIx, const char *line2);
void print_oled(SPI_TypeDef *SPIx, const char *line1, const char *line2);
void config_oled_gpio(void);
#endif

void init_oled_bb(void);
void send_oled_line1_bb(const char *line1);
void send__oled_line2_bb(const char *line2);
void print_oled_bb(const char *line1, const char *line2);
void config_oled_gpio_bb(void);
#endif /* SPI_OLED_H */


