/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.1.0   2018-11-25

The MIT License (MIT)
Copyright (c) 2018 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include "stm32f10x.h"

  int i = 0;

int main(void)
{
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9 | RCC_CFGR_PLLXTPRE);
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used.
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates
  *  SCB->VTOR register.
  *  E.g.  SCB->VTOR = 0x20000000;
  */

	/* Enable HSE */
	    RCC_HSEConfig( RCC_HSE_ON);





  RCC->APB2ENR = RCC_APB2ENR_IOPCEN | RCC_APB2ENR_TIM1EN;
  //RCC->CFGR |= 0x000338CD;
  RCC->CR |= 0x00010000;
  RCC->CR &= 0xFFFFFFF0;

//  AFIO->MAPR = AFIO_MAPR_PD01_REMAP;

  TIM1->CNT = 0x0003;
  TIM1->PSC= 10000;    //SETOU O VALOR DO PRE SCALER
  TIM1->ARR= 1000;   //NUMERO MAXIMO DO COUNTER
  TIM1->CR1=TIM_CR1_CEN | TIM_CR1_DIR; //ATIVOU O CONTADOR (ACIMA)
  GPIOC->CRH=0x44244444;
  GPIOC->ODR|=GPIO_ODR_ODR13;

  //Set LSE
  //RCC->BDCR |= 0x00010000;
  //RCC->BDCR &= 0xFFF1FFFF;


  /* Infinite loop */
  while (1)
  {
     if(TIM1->SR & TIM_SR_UIF)
         {
             TIM1->SR &= 0x33333330; // OU PODE SER FEITO: ~TIM_SR_UIF (INVERTE TODOS OS BITS)
             GPIOC->ODR ^= GPIO_ODR_ODR13; //FAZ UM XOR
             i++;
         }
     if(i >= 300){
    	 i=0;
     }
  }
}


/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}
