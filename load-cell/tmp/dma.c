#include "stm32f4xx.h"

void config_dma2_mem2mem(uint8_t *src, uint8_t *dest, uint16_t numBytes) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    
    DMA2_Stream0->CR = 0;
    while(DMA1_Stream0->CR &= DMA_SxCR_EN); /* wait for DMA to stop */

    DMA2_Stream0->CR |= (DMA_SxCR_CHSEL | /* Channel 7 */
                         DMA_SxCR_DIR_1); /* mem 2 mem */

    DMA2_Stream0->CR &= ~(DMA_SxCR_MBURST | /* no memory burst */
                          DMA_SxCR_PBURST | /* no peripheral burst */
                          DMA_SxCR_CT     | /* destination is M0AR */
                          DMA_SxCR_PL     | /* low priority */
                          DMA_SxCR_MSIZE  | /* 8-byte memory-side destination */
                          DMA_SxCR_PSIZE  | /* 8-byte peripheral source */
                          DMA_SxCR_PFCTRL); /* DMA controls */

    DMA2_Stream0->NDTR = numBytes; /* variable payload size */

    DMA2_Stream0->M0AR = (uint32_t)dest; /* destination address */
    DMA2_Stream0->PAR = (uint32_t) src; /* source address */


    DMA2_Stream0->CR |= DMA_SxCR_EN;
}