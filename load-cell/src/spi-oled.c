#include "stm32f4xx.h"
#include <stdint.h>
#include "clock.h"
#include "spi-oled.h"

static inline void nss_low(void) {
    GPIOC->BSRR |= GPIO_BSRR_BR_11;
}

static inline void nss_high(void) {
    GPIOC->BSRR |= GPIO_BSRR_BS_11;
}

static inline void sck_high(void) {
    GPIOC->BSRR |= GPIO_BSRR_BS_10;
}

static inline void sck_low(void) {
    GPIOC->BSRR |= GPIO_BSRR_BR_10;
}

static inline void mosi_high(void) {
    GPIOC->BSRR |= GPIO_BSRR_BS_12;
}

static inline void mosi_low(void) {
    GPIOC->BSRR |= GPIO_BSRR_BR_12;
}

static void send_bit_bb(uint8_t bit) {
    /* put proper MOSI bit on the line */
    if(bit == 0)
        mosi_low();
    else
        mosi_high();

    /* wait before posedge SCK */
    micro_wait(1);

    /* posedge SCK and then fall after 1us */
    sck_high();
    micro_wait(1);
    sck_low();      /* SCK just fell (MOSI should transition now) */
}

/* prefix with 2'b00 MSB-first */
static void send_cmd_bb(uint8_t cmd) {
    nss_low();
    micro_wait(1);
    
    send_bit_bb(0);
    send_bit_bb(0);
    for(uint8_t i = 0; i < 8; i++) {
        send_bit_bb((cmd >> (7 - i)) & 0x1);   
    }

    micro_wait(1);
    nss_high();
    micro_wait(1);
}

/* prefix with 2'b10 MSB-first */
static void send_data_bb(uint8_t data) {
    nss_low();
    micro_wait(1);
    
    send_bit_bb(1);
    send_bit_bb(0);
    for(uint8_t i = 0; i < 8; i++) {
        send_bit_bb(data & (1 << (7 - i)));   
    }

    micro_wait(1);
    nss_high();
    micro_wait(1);
}

/* Initialize the OLED via bit-banging */
void init_oled_bb(void) {
	send_cmd_bb(0x38); // Function Set: English/Japanese font, 8 bit data packets
    micro_wait(20000); /* delay 2ms according to datasheet */

    send_cmd_bb(0x08); // Display OFF, no cursor
	send_cmd_bb(0x01); // Display Clear: clears all characters from DDRAM, &DDRAM = 0x0
	
    micro_wait(50000); /* delay 5ms */
	
    send_cmd_bb(0x06); // Entry Mode Set: increment DDRAM address upon write to DDRAM
	send_cmd_bb(0x02); // Home Command: reset DDRAM to 0x0 and blank screen
	send_cmd_bb(0x0c); // Display ON: the last two bits set cursor to blink (Rick wants 0x0c)
}


/* Display line 1 on the OLED using SPI */
void send_oled_line1_bb(const char *line1) {
	send_cmd_bb(0x02); /* reset DDRAM address to beginning of line1 */
	for(uint8_t i = 0; i < MAX_CHARS_PER_LINE; i++) {
		if(*line1 != '\0') {
			send_data_bb(*line1++);
		}
		else {
			send_data_bb(' ');
		}
    }
}

/* Display line 2 on the OLED using SPI */
void send_oled_line2_bb(const char *line2) {
	send_cmd_bb(0xC0); /* reset DDRAM address to beginning of line2 */
	for(uint8_t i = 0; i < MAX_CHARS_PER_LINE; i++) {
		if(*line2 != '\0') {
			send_data_bb(*line2++);
		}
		else {
			send_data_bb(' ');
		}
    }
}

/* Print the characters to the OLED in accordance to the 
 * strings for lines 1 and 2, respecticely using CPU. */
void print_oled_bb(const char *line1, const char *line2) {
    send_oled_line1_bb(line1);
    send_oled_line2_bb(line2);
}

/* Set up the GPIO pins as for bit banging. I chose the pins
 * PC10, PC11, and PC12 for SCK, NSS, and MOSI, respectively. */
void config_oled_gpio_bb(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    /* configure the PC10, PC11, and PC12 for GP output */
    GPIOC->MODER &= ~(GPIO_MODER_MODER10 |
                      GPIO_MODER_MODER11 |
                      GPIO_MODER_MODER12);
    GPIOC->MODER |=  (GPIO_MODER_MODER10_0 | /* config all pins for output */
                      GPIO_MODER_MODER11_0 |
                      GPIO_MODER_MODER12_0);
                      
    /* remove biasing resistors and configure push-pull output */
    GPIOC->OTYPER &= ~(GPIO_OTYPER_IDR_10 |
                       GPIO_OTYPER_IDR_11 |
                       GPIO_OTYPER_IDR_12);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR10 |
                      GPIO_PUPDR_PUPDR11 |
                      GPIO_PUPDR_PUPDR12);

    /* assert slave select */
    nss_high();
}


#if 0

/* Initializes SPI3 with NSS = PA4, SCK = PC10, MOSI = PC12
 * for controlling the OLED SOC1602A character display. This
 * SPI peripheral will send data out at 16-bits at a time 
 * and will do so via CPU with a clock freqeuncy of 250kHz */
void config_spi_oled(SPI_TypeDef *SPIx) {
    switch((uint32_t)SPIx) {
        case((uint32_t)SPI1):
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            break;

        case (uint32_t)SPI2:
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;    
            break;
        case (uint32_t)SPI3:
            RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;    
            break;

        default:
            for(;;); /* invalid SPI base address */
    }

    /* configure the SPI register for data format */
    SPIx->CR1 = 0;

    SPIx->CR1 |= (SPI_CR1_DFF       |   /* 16-bit data frame */
                  (SPI_SCK_BR << 3)	|   /* divides the SPIx_CLK by 64 (500kHz SCK) */
				  SPI_CR1_SSM	    |   /* satisfy the SPI hardware */
				  SPI_CR1_SSI	    |
                  SPI_CR1_MSTR);    /* MCU is the SPI bus master */

    SPIx->CR2 = 0;

    /* enable the SPIx peripheral */
    SPIx->CR1 |= SPI_CR1_SPE;
}   

/* Send a 16-bit command (only 1 command byte + R/W bits) */
void send_oled_cmd8(SPI_TypeDef *SPIx, uint8_t commandByte) {
	/* wait for TX buffer to empty, else send data byte prefixed with 2'b00 */
    while(!(SPIx->SR & SPI_SR_TXE));

    nss_low();
    micro_wait(1);
	SPIx->DR = (uint16_t) commandByte;
    micro_wait(US_PER_BAUD * BAUDS_PER_TRANSMISSION + US_PER_BAUD);
    slave_select_high();
}

/* Send a byte of data MSB-first (prefixed with 2'b10) in 16-bit format */
void send_oled_data8(SPI_TypeDef *SPIx, uint8_t dataByte) { // prefix w/ 2'b10 for a DDRAM write operation
	while(!(SPIx->SR & SPI_SR_TXE));
    
    slave_select_low();
	SPIx->DR = (0x0200 | (uint16_t)dataByte); // 10 xxxx xxxx gets sent
    micro_wait(US_PER_BAUD * BAUDS_PER_TRANSMISSION + US_PER_BAUD);
    while(!(SPIx->SR & SPI_SR_TXE));
    slave_select_high();
}

/* Send a 10-bit command (2'b00 prefix + 1 byte) with bit-banging */
void send_oled_cmd8(SPI_TypeDef *SPIx, uint8_t commandByte) {
    slave_select_low();
    micro_wait(1);
	SPIx->DR = (uint16_t) commandByte;
    micro_wait(US_PER_BAUD * BAUDS_PER_TRANSMISSION + US_PER_BAUD);
    slave_select_high();
}

/* Send a byte of data MSB-first (prefixed with 2'b10) in 16-bit format */
void send_oled_data8(SPI_TypeDef *SPIx, uint8_t dataByte) { // prefix w/ 2'b10 for a DDRAM write operation
	while(!(SPIx->SR & SPI_SR_TXE));
    
    slave_select_low();
	SPIx->DR = (0x0200 | (uint16_t)dataByte); // 10 xxxx xxxx gets sent
    micro_wait(US_PER_BAUD * BAUDS_PER_TRANSMISSION + US_PER_BAUD);
    while(!(SPIx->SR & SPI_SR_TXE));
    slave_select_high();
}

void spi_init_oled(SPI_TypeDef *SPIx) {
	send_oled_cmd8_bb(0x38); // Function Set: English/Japanese font, 8 bit data packets
    micro_wait(2000); /* delay 2ms according to datasheet */

    send_oled_cmd8(SPIx, 0x08); // Display OFF, no cursor
	send_oled_cmd8(SPIx, 0x01); // Display Clear: clears all characters from DDRAM, &DDRAM = 0x0
	
    micro_wait(5000); /* delay 5ms */
	
    send_oled_cmd8(SPIx, 0x06); // Entry Mode Set: increment DDRAM address upon write to DDRAM
	send_oled_cmd8(SPIx, 0x02); // Home Command: reset DDRAM to 0x0 and blank screen
	send_oled_cmd8(SPIx, 0x0c); // Display ON: the last two bits set cursor to blink (Rick wants 0x0c)
}

/* Display line 1 on the OLED using SPI */
void send_oled_line1(SPI_TypeDef *SPIx, const char *line1) {
    int8_t maxCharsLeft = MAX_CHARS_PER_LINE;

	send_oled_cmd8(SPIx, 0x02); /* reset DDRAM address to beginning of line1 */
	while( (--maxCharsLeft != 0) && (*line1 != '\0') ) {
		send_oled_data8(SPIx, *line1++);
    }
}

/* Display line 2 on the OLED using SPI */
void send_oled_line2(SPI_TypeDef *SPIx, const char *line2) {
    int8_t maxCharsLeft = MAX_CHARS_PER_LINE;

	send_oled_cmd8(SPIx, 0xC0); /* reset DDRAM address to beginning of line2 */
	while( (--maxCharsLeft != 0) && (*line2 != '\0') ) {
		send_oled_data8(SPIx, *line2++);
    }

}

/* Print the characters to the OLED in accordance to the 
 * strings for lines 1 and 2, respecticely using CPU. */
void print_oled(SPI_TypeDef *SPIx, const char *line1, const char *line2) {
    send_oled_line1(SPIx, line1);
    send_oled_line2(SPIx, line2);
}

/* Set up the GPIO pins as user wants. For SPI3 I am currently using:
 * 1. SCK = PC10 (AF6)  2. MOSI = PC12 (AF6)  3. NSS = PC11 (AF0) */
void config_oled_gpio(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    /* configure the PA4, PC10, and PC12 for alternate function */
    GPIOC->MODER &= ~(GPIO_MODER_MODER10 |
                      GPIO_MODER_MODER11 |
                      GPIO_MODER_MODER12);
    GPIOC->MODER |=  (GPIO_MODER_MODER10_1 | /* config GPIO for AF or output */
                      GPIO_MODER_MODER11_0 |
                      GPIO_MODER_MODER12_1);
                      
    /* remove biasing resistors and configure push-pull output */
    GPIOC->OTYPER &= ~(GPIO_OTYPER_IDR_10 |
                       GPIO_OTYPER_IDR_11 |
                       GPIO_OTYPER_IDR_12);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR10 |
                      GPIO_PUPDR_PUPDR11 |
                      GPIO_PUPDR_PUPDR12);

    /* configure PC10 (SCK) and PC12 (MOSI) for AF6 */
    GPIOC->AFR[1] &= ~(0xF << (BITS_PER_AF_PIN * (SCK_PIN_NUMBER & 0x7)));
    GPIOC->AFR[1] |= (0x6 << (BITS_PER_AF_PIN * (SCK_PIN_NUMBER & 0x7)));

    GPIOC->AFR[1] &= ~(0xF << (BITS_PER_AF_PIN * (MOSI_PIN_NUMBER & 0x7)));
    GPIOC->AFR[1] |= (0x6 << (BITS_PER_AF_PIN * (MOSI_PIN_NUMBER & 0x7)));

    /* activate NSS to be active high */
    slave_select_high();
}

#endif /* BAD SPI for OLED */




