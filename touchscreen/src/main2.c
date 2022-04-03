///**
//  ******************************************************************************
//  * @file    main.c
//  * @author  Ac6
//  * @version V1.0
//  * @date    01-December-2013
//  * @brief   Default main function.
//  ******************************************************************************
//*/
//
//
//#include "stm32f4xx.h"
//#include "ili9341.h"
//
//
//
//#define CD_COMMAND GPIOA -> BSRRH |= GPIO_BSRR_BS_6     // Set PA6 low
//#define CD_DATA    GPIOA -> BSRRL |= GPIO_BSRR_BS_6     // Set PA6 high
//#define CS_ACTIVE  GPIOB -> BSRRH |= GPIO_BSRR_BS_9     // Set PB9 low
//#define CS_IDLE    GPIOB -> BSRRL |= GPIO_BSRR_BS_9     // Set PB9 high
//#define RST_ACTIVE GPIOB -> BSRRH |= GPIO_BSRR_BS_6     // Set PB6 low
//#define RST_IDLE   GPIOB -> BSRRL |= GPIO_BSRR_BS_6     // Set PB6 high
//
//#define TFTWIDTH 240
//#define TFTHEIGHT 320
//
//
//
//void setCDline() {
//    // CD line
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//    GPIO_InitTypeDef GPIO_InitStruct;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOA, &GPIO_InitStruct);
//}
//
//
//
//// Initializes the SPI2 subsystem and connects its NSS, SCK, and MOSI to PB9, PB10, and PC3
//void setup_spi2() {
//    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //enable port b
//    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //enable port c
//    RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN; //enable SPI2
//
//    // Set to alternate function
////    GPIOB -> MODER &= ~GPIO_MODER_MODER9;
////    GPIOB -> MODER |= GPIO_MODER_MODER9_1;
//    GPIOB -> MODER &= ~GPIO_MODER_MODER9;
//    GPIOB -> MODER |= GPIO_MODER_MODER9_0; // set to output
//    GPIOB -> BSRRL |=  GPIO_BSRR_BS_9; //set idle high
//
//    GPIOB -> MODER &= ~GPIO_MODER_MODER10;
//    GPIOB -> MODER |= GPIO_MODER_MODER10_1;
//    GPIOC -> MODER &= ~GPIO_MODER_MODER3;
//    GPIOC -> MODER |= GPIO_MODER_MODER3_1;
//
//    // Connect NSS, SCK, and MOSI signals to pins PB9, PB10, and PC3, respectively
//    GPIOB -> AFR[1] &= ~(0xf << 4); // PB9: AF5 = SPI2_NSS (0101)
//    GPIOB -> AFR[1] |= (0x5 << 4);
//    GPIOB -> AFR[1] &= ~(0xf << 8); // PB10: AF5 = SPI2_SCK (0101)
//    GPIOB -> AFR[1] |= (0x5 << 8);
//    GPIOC -> AFR[0] &= ~(0xf << 12); // PC3: AF5 = SPI2_MOSI (0101)
//    GPIOC -> AFR[0] |= (0x5 << 12);
//
//    // Configure SPI2
////    SPI2 -> CR1 |= SPI_CR1_BR; // Set baud rate as low as possible (x MHz / 256)
//    SPI2 -> CR1 &= ~SPI_CR1_BR; //set at fast as possible
//
//    // default set for 8 bit word size
//    SPI2 -> CR1 |= SPI_CR1_MSTR; // Configure to be in master mode
//    SPI2 -> CR2 |= SPI_CR2_SSOE; // Set the SS Output enable bit
//    SPI2 -> CR1 |= SPI_CR1_SPE; // Enable the SPI channel
//}
//
//void spi_send(uint8_t n) {
//    while ((SPI2 -> SR & SPI_SR_TXE) == 0); //Wait until the SPI2_SR_TXE bit is set
//    SPI2 -> DR = n;
//}
//
//
//void send_cmd8(uint8_t d) {
//    CD_COMMAND;
//    CS_ACTIVE;
//    spi_send(d);
//    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
//    CD_DATA;
////    CS_IDLE;
//}
//
//void send_data8(uint8_t d) {
//    CD_DATA;
//    CS_ACTIVE;
//    spi_send(d);
//    while ((SPI2 -> SR & 1 << 7) != 0); // Wait for transaction to complete
////    CS_IDLE;
//}
//
////void writeRegister8(uint8_t c, uint8_t d) {
////    send_cmd8(c);
////    send_data8(d);
////}
////
////void writeRegister16(uint16_t c, uint16_t d) {
////    uint8_t hi, lo;
////    hi = (c) >> 8;
////    lo = (c);
////    send_cmd8(hi);
////    send_cmd8(lo);
////
////    hi = (d) >> 8;
////    lo = (d);
////    send_data8(hi);
////    send_data8(lo);
////}
////
////void writeRegister32(uint8_t r, uint32_t d) {
////    send_cmd8(r);
////    delay(10);
////    send_data8(d >> 24);
////    delay(10);
////    send_data8(d >> 16);
////    delay(10);
////    send_data8(d >> 8);
////    delay(10);
////    send_data8(d);
////}
//
//void delay(int d) {
//    for (int i = 0; i < d * 100; i++);
//}
//
//#define INIT_SENTINEL 0x00U /* stop commands/data upon reaching cmd 0x00 */
//static uint8_t lcdInitList[111] = {
//    0xEF, 3, 0x03, 0x80, 0x02,
//    0xCF, 3, 0x00, 0xC1, 0x30,
//    0xED, 4, 0x64, 0x03, 0x12, 0x81,
//    0xE8, 3, 0x85, 0x00, 0x78,
//    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
//    0xF7, 1, 0x20,
//    0xEA, 2, 0x00, 0x00,
//    0xC0, 1, 0x23,              // Power control VRH[5:0]
//    0xC1, 1, 0x10,              // Power control SAP[2:0];BT[3:0]
//    0xC5, 2, 0x3e, 0x28,        // VCM control
//    0xC7, 1, 0x86,              // VCM control2
//    0x36, 1, 0x48,              // Memory Access Control
//    0x37, 1, 0x00,              // Vertical scroll zero
//    0x3A, 1, 0x55,              // Pixel Format Set
//    0xB1, 2, 0x00, 0x18,
//    0xB6, 3, 0x08, 0x82, 0x27,  // Display Function Control
//    0xF2, 1, 0x00,                             // 3Gamma Function Disable
//    0x26, 1, 0x01,             // Gamma curve selected
//    0xE0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
//              0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
//    0xE1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
//              0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
//    0x11, 0,         // Exit Sleep (and delay 5ms)
//    0x29, 0,         // Display on (and delay 5ms)
//    INIT_SENTINEL             // 0x00 is the init_lcd() sentinel (NOT sent)
//};
//
//
//
//
//void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
//    send_cmd8(0x2a); // begin start and end column addressing
//
////    uint8_t xDataArr[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xff}; // little endian {x0, x1}
//    send_data8(x0 >> 8);
//    send_data8(x0 & 0xff);
//    send_data8(x1 >> 8);
//    send_data8(x1 & 0xff);
////    send_data_bytes(SPIx, xDataArr, sizeof(xDataArr));
//
//    send_cmd8(0x2b); // begin start and end row addressing
//
//
////    uint8_t yDataArr[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xff}; // little endian {y0, y1}
//    send_data8(y0 >> 8);
//    send_data8(y0 & 0xff);
//    send_data8(y1 >> 8);
//    send_data8(y1 & 0xff);
//    //    send_data_bytes(SPIx, yDataArr, sizeof(yDataArr));
//
//    send_cmd8(0x2c);
////    send_cmd_byte_lcd(SPIx, 0x2c); // prepares display RAM for writing
//}
//
//
///* draws a box on the LCD using the same color and via CPU */
//void lcd_draw_box_color_cpu(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
//    lcd_set_window(x, y, (x + width - 1), (y + height - 1));
//
//    /* write all of the RGB pixel data to RAM using CPU */
//    for(int i=0; i < width*height; i++) {
//        send_data8(color);
//    }
//}
//
//
//
//
//int main(void)
//{
//    setCDline();
//    setup_spi2();
//
//    // Soft reset
//    send_cmd8(0x01);
//    delay(50);
//
//    uint8_t *data = lcdInitList;
//    uint8_t num_data;
//    while (*data != INIT_SENTINEL) {
//        send_cmd8(*data);
//        data++;
//        num_data = *data;
//        data++;
//        for (uint8_t i = 0; i < num_data; i++) {
//            send_data8(*data);
//            data++;
//        }
//    }
//
//    lcd_draw_box_color_cpu(100, 100, 100, 100, 0xf0);
//
//
//
//
//
//
//
//
////    setCDline();
////    normal_SPI();
////
////    CS_ACTIVE;
////    sendCommand(0xab);
////
////    CS_IDLE;
//
//
////    ili9341_Init();
////    ili9341_DisplayOn();
////    ili9341_FillRect(100,100,100,100, 0x000);
////    for(;;);
//}
