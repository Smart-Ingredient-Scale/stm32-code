// #include "stm32f4xx.h"
// #include "touchscreen.h"



// void init_ts(void)
// {
//    // PC4 = ADC_IN14, PB0 = ADC_IN8
//    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //enable clock
//    RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

//    // Set PC4 and PB0 for analog (11)
//    GPIOC -> MODER |= GPIO_MODER_MODER4;
//    GPIOB -> MODER |= GPIO_MODER_MODER0;

//    RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN; //enable the clock to the ADC

//    ADC1 -> SMPR2 &= ~ADC_SMPR2_SMP8;
//    ADC1 -> SMPR2 |= ADC_SMPR2_SMP8_1; //010 - 28 cycles
   

// //    RCC -> CR2 |= RCC_CR2_HSI14ON; //turn on the "high-speed internal" 14 MHz clock (HSI14)
// //    while (!(RCC -> CR2 & RCC_CR2_HSI14RDY)); //wait for 14 MHz clock to be ready (turn bit to 1)

//    ADC1 -> CR2 |= ADC_CR2_ADON; //enable the ADC
// //    while (!(ADC1 -> ISR & ADC_ISR_ADRDY)); //wait for the ADC to be ready

// //    //maybe?
// //    while((ADC1 -> CR & ADC_CR_ADSTART)); //wait for ADCstart to be 0

//    //  In the event that special operating conditions (DMA, continuous conversion mode, etc.) need to be performed by the ADC,
//    //operating modes can be specified via the ADC configuration registers: ADC_CFGR1 and ADC_CFGR2
// }


// // /* CS = 1, X+ = 0, X- = 0; Y+ = in PU, Y- = in PU */
// // uint8_t TS_IO_DetectTouch(void)
// // {
// //  uint8_t  ret;

// //  GPIOX_MODER(MODE_DIGITAL_INPUT, TS_YP);/* YP = D_INPUT */
// //  GPIOX_MODER(MODE_DIGITAL_INPUT, TS_YM);/* YM = D_INPUT */
// //  GPIOX_ODR(TS_XP) = 0;                 /* XP = 0 */
// //  GPIOX_ODR(TS_XM) = 0;                 /* XM = 0 */

// //  /* pullup resistor on */
// //  GPIOX_PUPDR(MODE_PU_UP, TS_YP);

// //  LCD_IO_Delay(TS_AD_DELAY);

// //  if(GPIOX_IDR(TS_YP))
// //    ret = 0;                            /* Touchscreen is not touch */
// //  else
// //    ret = 1;                            /* Touchscreen is touch */

// //  /* pullup resistor off */
// //  GPIOX_PUPDR(MODE_PU_NONE, TS_YP);

// //  GPIOX_ODR(TS_XP) = 1;                 /* XP = 1 */
// //  GPIOX_ODR(TS_XM) = 1;                 /* XM = 1 */
// //  GPIOX_MODER(MODE_OUT, TS_YP);         /* YP = OUT */
// //  GPIOX_MODER(MODE_OUT, TS_YM);         /* YM = OUT */

// //  return ret;
// // }

// //-----------------------------------------------------------------------------
// /* read the X position */
// uint16_t TS_IO_GetX(void)
// {
//     uint16_t ret;

//     TS_YM_MODE_INPUT; /* YM = D_INPUT */
//     TS_YP_MODE_ANALOG; /* YP = AN_INPUT */
//    //  GPIOB -> 

//     TS_XM_MODE_OUTPUT;
//     TS_XP_MODE_OUTPUT;
//     TS_XM_HIGH; /* XP = 0 */
//     TS_XP_LOW; /* XM = 1 */
//     // GPIOX_MODER(MODE_DIGITAL_INPUT, TS_YM);
//     // GPIOX_MODER(MODE_ANALOG_INPUT, TS_YP); 
//     // GPIOX_ODR(TS_XP) = 0;                 
//     // GPIOX_ODR(TS_XM) = 1;       

//     ADC1->CR1 &= ~ADC_CR1_AWDCH;
//     ADC1->CR1 |= ADC_CR1_AWDCH_3; // PB0: ADC1_IN8      


//     ADC1->CR2 |= ADC_CR2_SWSTART; //Start
//     while((ADC1->SR & ADC_SR_EOC) == 0); // Wait for end of conversion
// //    for(int i = 0; i < 1000; i++);
//     ret = ADC1->DR; //?, 12 bit precision

//     TS_XP_HIGH; /* XP = 1 */
//     TS_YP_MODE_OUTPUT;
//     TS_YM_MODE_OUTPUT;
//     //

//    //  ADC1->SQR3 = TS_YP_ADCCH;
//    //  LCD_IO_Delay(TS_AD_DELAY);
//    //  ADC1->CR2 |= ADC_CR2_SWSTART;
//    //  while(!(ADC1->SR & ADC_SR_EOC));
//    //  ret = ADC1->DR;

//    //  GPIOX_ODR(TS_XP) = 1;                 /* XP = 1 */
//    //  GPIOX_MODER(MODE_OUT, TS_YP);
//    //  GPIOX_MODER(MODE_OUT, TS_YM);

//     return ret;
// }

// // //-----------------------------------------------------------------------------
// // /* read the Y position */
// // uint16_t TS_IO_GetY(void)
// // {
// //  uint16_t ret;

// //  GPIOX_MODER(MODE_DIGITAL_INPUT, TS_XP);/* XP = D_INPUT */
// //  GPIOX_MODER(MODE_ANALOG_INPUT, TS_XM); /* XM = AN_INPUT */
// //  GPIOX_ODR(TS_YM) = 0;                 /* YM = 0 */
// //  GPIOX_ODR(TS_YP) = 1;                 /* YP = 1 */

// //  ADCX->SQR3 = TS_XM_ADCCH;
// //  LCD_IO_Delay(TS_AD_DELAY);
// //  ADCX->CR2 |= ADC_CR2_SWSTART;
// //  while(!(ADCX->SR & ADC_SR_EOC));
// //  ret = ADCX->DR;

// //  GPIOX_ODR(TS_YM) = 1;                 /* YM = 1 */
// //  GPIOX_MODER(MODE_OUT, TS_XP);
// //  GPIOX_MODER(MODE_OUT, TS_XM);

// //  return ret;
// // }

// // //-----------------------------------------------------------------------------
// // /* read the Z1 position */
// // uint16_t TS_IO_GetZ1(void)
// // {
// //  uint16_t ret;

// //  GPIOX_MODER(MODE_ANALOG_INPUT, TS_XM); /* XM = AN_INPUT */
// //  GPIOX_MODER(MODE_ANALOG_INPUT, TS_YP); /* YP = AN_INPUT */
// //  GPIOX_ODR(TS_XP) = 0;                 /* XP = 0 */
// //  GPIOX_ODR(TS_YM) = 1;                 /* YM = 1 */

// //  ADCX->SQR3 = TS_YP_ADCCH;
// //  LCD_IO_Delay(TS_AD_DELAY);
// //  ADCX->CR2 |= ADC_CR2_SWSTART;
// //  while(!(ADCX->SR & ADC_SR_EOC));
// //  ret = ADCX->DR;

// //  GPIOX_ODR(TS_XP) = 1;                 /* XP = 1 */
// //  GPIOX_MODER(MODE_OUT, TS_XM);
// //  GPIOX_MODER(MODE_OUT, TS_YP);

// //  return ret;
// // }

// // //-----------------------------------------------------------------------------
// // /* read the Z2 position */
// // uint16_t TS_IO_GetZ2(void)
// // {
// //  uint16_t ret;

// //  GPIOX_MODER(MODE_ANALOG_INPUT, TS_XM); /* XM = AN_INPUT */
// //  GPIOX_MODER(MODE_ANALOG_INPUT, TS_YP); /* YP = AN_INPUT */
// //  GPIOX_ODR(TS_XP) = 0;                 /* XP = 0 */
// //  GPIOX_ODR(TS_YM) = 1;                 /* YM = 1 */

// //  ADCX->SQR3 = TS_XM_ADCCH;
// //  LCD_IO_Delay(TS_AD_DELAY);
// //  ADCX->CR2 |= ADC_CR2_SWSTART;
// //  while(!(ADCX->SR & ADC_SR_EOC));
// //  ret = ADCX->DR;

// //  GPIOX_ODR(TS_XP) = 1;                 /* XP = 1 */
// //  GPIOX_MODER(MODE_OUT, TS_XM);
// //  GPIOX_MODER(MODE_OUT, TS_YP);

// //  return ret;
// // }
