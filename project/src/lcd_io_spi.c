#include "lcd_io_spi.h"
#include "stm32f4xx.h"
#include "lcd.h"

#include "clock.h"


#define TFTWIDTH 240
#define TFTHEIGHT 320

// Init commands 

// Internal Functions //
static void setCDline() {
   // CD line
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
   GPIO_InitTypeDef GPIO_InitStruct;
   GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// Initializes the SPI2 subsystem and connects the following
// NSS: PB12 (OUT mode)
// SCK: PB13
// MOSI: PB15
static void setup_spi2() {
   RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //enable port b
   RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //enable port c
   RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN; //enable SPI2

   // NSS is in OUT mode
   GPIOB -> MODER &= ~GPIO_MODER_MODER12;
   GPIOB -> MODER |= GPIO_MODER_MODER12_0; // set to output
   GPIOB -> BSRR |=  GPIO_BSRR_BS_12; //set idle high
//    GPIOB -> BSRRL |=  GPIO_BSRR_BS_12; //set idle high

   // Set to alternate function
   GPIOB -> MODER &= ~GPIO_MODER_MODER13;
   GPIOB -> MODER |= GPIO_MODER_MODER13_1;
   GPIOB -> MODER &= ~GPIO_MODER_MODER15;
   GPIOB -> MODER |= GPIO_MODER_MODER15_1;

   // Connect SCK and MOSI signals
   GPIOB -> AFR[1] &= ~(0xf << 20); // PB13: AF5 = SPI2_SCK (0101)
   GPIOB -> AFR[1] |= (0x5 << 20);
   GPIOB -> AFR[1] &= ~(0xf << 28); // PB15: AF5 = SPI2_MOSI (0101)
   GPIOB -> AFR[1] |= (0x5 << 28);

   // Configure SPI2
   SPI2 -> CR1 |= SPI_CR1_BR; // Set baud rate as low as possible (x MHz / 256)
   //SPI2 -> CR1 &= ~SPI_CR1_BR; //set at fast as possible

   // default set for 8 bit word size
   SPI2 -> CR1 |= SPI_CR1_MSTR; // Configure to be in master mode
   SPI2 -> CR2 |= SPI_CR2_SSOE; // Set the SS Output enable bit
   SPI2 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
}


static void spi_send8(uint8_t n) {
   while ((SPI2 -> SR & SPI_SR_TXE) == 0); //Wait until the SPI2_SR_TXE bit is set
   SPI2 -> DR = n;
   micro_wait(150);
}

static void spi_send16(uint16_t n) {
   while ((SPI2 -> SR & SPI_SR_TXE) == 0); //Wait until the SPI2_SR_TXE bit is set
   SPI2 -> DR = n;
   micro_wait(150);
}


static void send_cmd8(uint8_t d) {
   CD_COMMAND;
   CS_ACTIVE;
   spi_send8(d);
//    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
   micro_wait(1);
//    for (int i = 0; i < 1000; i++);
   CD_DATA;
CS_IDLE;
}

static void send_data8(uint8_t d) {
   CD_DATA;
   CS_ACTIVE;
   spi_send8(d);
//    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
   micro_wait(1);
//    for (int i = 0; i < 1000; i++);
   CS_IDLE;
}

static void LcdSpiMode8() {
    SPI2 -> CR1 &= ~SPI_CR1_SPE; // Disable the SPI channel
    SPI2 -> CR1 &= ~SPI_CR1_DFF;
    SPI2 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
}

static void LcdSpiMode16() {
    SPI2 -> CR1 &= ~SPI_CR1_SPE; // Disable the SPI channel
    SPI2 -> CR1 |= SPI_CR1_DFF;
    SPI2 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
}












// External Facing Functions //
void LCD_Delay(uint32_t delay) {
   for (int i = 0; i < delay * 1000; i++);
    // micro_wait(delay * 10);
}

void LCD_IO_Init(void) {
    setCDline();
    setup_spi2();

    // // // Soft reset
    // send_cmd8(0x01);
    // LCD_Delay(50); //delay(50);

    // // Init sequence
    // uint8_t *data = lcdInitList;
    // uint8_t num_data;
    // while (*data != INIT_SENTINEL) {
    //     send_cmd8(*data);
    //     data++;
    //     num_data = *data;
    //     data++;
    //     for (uint8_t i = 0; i < num_data; i++) {
    //         send_data8(*data);
    //         data++;
    //     }
    // }
}

void  LCD_IO_Bl_OnOff(uint8_t bl) {

}

void LCD_IO_WriteCmd8(uint8_t cmd) {
    LcdSpiMode8();
    send_cmd8(cmd);
}

void LCD_IO_WriteData8(uint8_t data) {
    LcdSpiMode8();
    send_data8(data);
}

void LCD_IO_WriteCmd16(uint16_t cmd) {
    LcdSpiMode16();

    CD_COMMAND;
    CS_ACTIVE;
    spi_send16(cmd);
    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
    CD_DATA;
}

void LCD_IO_WriteData16(uint16_t data) {
    LcdSpiMode16();

    CD_DATA;
    CS_ACTIVE;
    spi_send16(data);
    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
}

void LCD_IO_WriteCmd8DataFill16(uint8_t cmd, uint16_t data, uint32_t size)
{
    LCD_IO_WriteCmd8(cmd);
    while(size--) {
        LCD_IO_WriteData16(data);
    }
}

void LCD_IO_WriteCmd8MultipleData8(uint8_t cmd, uint8_t *pData, uint32_t size) {
    LCD_IO_WriteCmd8(cmd);
    while(size--) {
        LCD_IO_WriteData8(*pData);
        pData++;
    }    
}

void LCD_IO_WriteCmd8MultipleData16(uint8_t cmd, uint16_t *pData, uint32_t size) {
    LCD_IO_WriteCmd8(cmd);
    while(size--) {
        LCD_IO_WriteData16(*pData);
        pData++;
    }    
}


void LCD_IO_WriteCmd16DataFill16(uint16_t cmd, uint16_t data, uint32_t size)
{
    LCD_IO_WriteCmd16(cmd);
    while(size--) {
        LCD_IO_WriteData16(data);
    }
}

void LCD_IO_WriteCmd16MultipleData8(uint16_t cmd, uint8_t *pData, uint32_t size) {
    LCD_IO_WriteCmd16(cmd);
    while(size--) {
        LCD_IO_WriteData8(*pData);
        pData++;
    }    
}

void LCD_IO_WriteCmd16MultipleData16(uint16_t cmd, uint16_t *pData, uint32_t size) {
    LCD_IO_WriteCmd16(cmd);
    while(size--) {
        LCD_IO_WriteData16(*pData);
        pData++;
    }    
}
