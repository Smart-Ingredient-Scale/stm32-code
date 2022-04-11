#include "stm32f4xx.h"
#include "spi-ss.h"
#include <stdlib.h>

// Initializes the SPI1 subsystem and connects its NSS, SCK, and MOSI to PA4, PA5, and PA7
void setup_spi1() {
    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //enable port a
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN; //enable SPI1

    GPIOA -> MODER &= ~GPIO_MODER_MODER4;
    GPIOA -> MODER |= GPIO_MODER_MODER4_0;
    GPIOA -> MODER &= ~GPIO_MODER_MODER5;
    GPIOA -> MODER |= GPIO_MODER_MODER5_1;
    GPIOA -> MODER &= ~GPIO_MODER_MODER7;
    GPIOA -> MODER |= GPIO_MODER_MODER7_1;

    // Connect NSS, SCK, and MOSI signals to pins PA4, PA5, and PA7, respectively
    GPIOA -> AFR[0] &= ~(0xf << 20); // PA5: AF5 = SPI1_SCK (0101)
    GPIOA -> AFR[0] |= (0x5 << 20);
    GPIOA -> AFR[0] &= ~(0xf << 28); // PC3: AF5 = SPI1_MOSI (0101)
    GPIOA -> AFR[0] |= (0x5 << 28);
    GPIOA -> BSRR |= GPIO_BSRR_BS_4; // Set NSS high

    // Configure SPI1
    SPI1 -> CR1 |= SPI_CR1_BR; // Set baud rate as low as possible (x MHz / 256)
    SPI1 -> CR1 |= SPI_CR1_DFF;  // set for 16 bit word size
    SPI1 -> CR1 |= SPI_CR1_MSTR; // Configure to be in master mode
    SPI1 -> CR2 |= SPI_CR2_SSOE; // Set the SS Output enable bit
    SPI1 -> CR1 |= SPI_CR1_SPE;  // Enable the SPI channel
}

void spi_send(uint16_t n) {
    // while ((SPI1 -> SR & SPI_SR_TXE) == 0); //Wait until the SPI2_SR_TXE bit is set
    GPIOA -> BSRR |= GPIO_BSRR_BR_4; // Set the NSS low
    SPI1 -> DR = n;
    for (int i = 0; i < 100; i++); // really lazy way to wait
    while ((SPI1 -> SR & 1 << 7) != 0); // Wait for transaction to complete
    GPIOA -> BSRR |= GPIO_BSRR_BS_4; // Set the NSS high
}

void test_ss_display(){
    spi_send(0xF01);   // Display-test register format (OVERRIDE SHUTDOWN MODE)
}

void off_display(){
    spi_send(0xF00);   // Normal operation mode
    spi_send(0xC00);   // Turn it off
}

void init_display(){
    spi_send(0xF00);   // Normal operation mode
    spi_send(0x0A04);  // Set Intensity to 31/32
    spi_send(0x0B07);  // Display digits 0 1 2 3 4 5 6 7
    spi_send(0x9FF);   // Decode mode for digits 0-7
    spi_send(0xC01);   // Turn it on
}

void ss_display_num(int32_t n){
    uint16_t digitReg;
    uint8_t digit;

    // If negative number then add "-" in left most digit, else BLANK
    if(n < 0) {
        spi_send(0x80A);
        n *= -1;
    }
    else spi_send(0x80F);

    for(int i = 0; i < 7; i++){
        digitReg = 0x100 + 0x100 * i;
        digit = n % 10;
        digitReg += digit;
        if(i == 2)
        {
            digitReg += 0x80; // Add decimal point
        }
        spi_send(digitReg);
        n /= 10;
    }
}
