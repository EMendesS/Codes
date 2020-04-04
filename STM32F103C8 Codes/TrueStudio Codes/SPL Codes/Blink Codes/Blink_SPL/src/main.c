#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "SysTick.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

//.....................GLOBAL.................................
int i = 0;
//uint32_t SystemCoreClock = 0;
//............................................................

void clock_init(){
  /*Configure all clocks to max for best performance.
   * If there are EMI, power, or noise problems, try slowing the clocks*/

  /* First set the flash latency to work with our clock*/
  /*000 Zero wait state, if 0  MHz < SYSCLK <= 24 MHz
    001 One wait state, if  24 MHz < SYSCLK <= 48 MHz
    010 Two wait states, if 48 MHz < SYSCLK <= 72 MHz */
  FLASH_SetLatency(FLASH_Latency_1);

  /* Start with HSI clock (internal 8mhz), divide by 2 and multiply by 9 to
   * get maximum allowed frequency: 36Mhz
   * Enable PLL, wait till it's stable, then select it as system clock*/
  RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);
  RCC_PLLCmd(ENABLE);
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Set HCLK, PCLK1, and PCLK2 to SCLK (these are default */
  RCC_HCLKConfig(RCC_SYSCLK_Div2);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_PCLK2Config(RCC_HCLK_Div1);

  /* Set ADC clk to 9MHz (14MHz max, 18MHz default)*/
  RCC_ADCCLKConfig(RCC_PCLK2_Div4);

  /*To save power, use below functions to stop the clock to ceratin
   * peripherals
   * RCC_AHBPeriphClockCmd
   */
}

void Initialize_Ports(void){

	/* GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpioStructure;

    //Config PC13
    gpioStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpioStructure);

    gpioStructure.GPIO_Pin = GPIO_Pin_6;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &gpioStructure);
}


int main()
{
	SystemInit();

/*Seta cristal externo 16MHz*/
//	SetSysClockTo72();

/*Seta cristal interno HSI*/
//	clock_init();
	Initialize_Ports();
	SysTick_Init();

    while(1){

    	GPIO_SetBits(GPIOC, GPIO_Pin_15);
    	GPIO_SetBits(GPIOA, GPIO_Pin_6);
    	delayUs(1000000);
    	GPIO_ResetBits(GPIOC, GPIO_Pin_15);
    	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    	delayUs(1000000);
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
