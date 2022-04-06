#ifndef _LCD_IO_SPI_H_
#define _LCD_IO_SPI_H_
#include "stm32f4xx.h"


#define CD_COMMAND GPIOC -> BSRR |= GPIO_BSRR_BR_6     // Set PC6 low
#define CD_DATA    GPIOC -> BSRR |= GPIO_BSRR_BS_6     // Set PC6 high
#define CS_ACTIVE  GPIOB -> BSRR |= GPIO_BSRR_BR_12    // Set PB12 low
#define CS_IDLE    GPIOB -> BSRR |= GPIO_BSRR_BS_12    // Set PB12 high
#define RST_ACTIVE GPIOC -> BSRR |= GPIO_BSRR_BR_7     // Set PC7 low
#define RST_IDLE   GPIOC -> BSRR |= GPIO_BSRR_BS_7     // Set PC7 high

void  LCD_Delay (uint32_t delay);
void  LCD_IO_Init(void);
void  LCD_IO_Bl_OnOff(uint8_t Bl); // TODO: backlight

void  LCD_IO_WriteCmd8(uint8_t Cmd);
void  LCD_IO_WriteCmd16(uint16_t Cmd);
void  LCD_IO_WriteData8(uint8_t Data);
void  LCD_IO_WriteData16(uint16_t Data);

void  LCD_IO_WriteCmd8DataFill16(uint8_t Cmd, uint16_t Data, uint32_t Size);
void  LCD_IO_WriteCmd8MultipleData8(uint8_t Cmd, uint8_t *pData, uint32_t Size);
void  LCD_IO_WriteCmd8MultipleData16(uint8_t Cmd, uint16_t *pData, uint32_t Size);
void  LCD_IO_WriteCmd16DataFill16(uint16_t Cmd, uint16_t Data, uint32_t Size);
void  LCD_IO_WriteCmd16MultipleData8(uint16_t Cmd, uint8_t *pData, uint32_t Size);
void  LCD_IO_WriteCmd16MultipleData16(uint16_t Cmd, uint16_t *pData, uint32_t Size);

// void  LCD_IO_ReadCmd8MultipleData8(uint8_t Cmd, uint8_t *pData, uint32_t Size, uint32_t DummySize);
// void  LCD_IO_ReadCmd8MultipleData16(uint8_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
// void  LCD_IO_ReadCmd8MultipleData24to16(uint8_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
// void  LCD_IO_ReadCmd16MultipleData8(uint16_t Cmd, uint8_t *pData, uint32_t Size, uint32_t DummySize);
// void  LCD_IO_ReadCmd16MultipleData16(uint16_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);
// void  LCD_IO_ReadCmd16MultipleData24to16(uint16_t Cmd, uint16_t *pData, uint32_t Size, uint32_t DummySize);

void  LCD_IO_Delay(uint32_t c);

#endif //_LCD_IO_SPI_H_
