/*
  ******************************************************************************
  * @file		delay.h
  * @author	Yohanes Erwin Setiawan
  * @date		10 January 2016
  ******************************************************************************
  */

#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

// For store tick counts in us
static volatile uint32_t usTicks;	// For store tick counts in us
static volatile uint32_t usTime = 0;		//For store time counter in us
static volatile uint32_t usTime_aux = 0;		//For store time counter in us (auxiliar variable)

//............................Set System Clock to 72MHz.........................

void SetSysClockTo72(void)
{
	ErrorStatus HSEStartUpStatus;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_HSEConfig( RCC_HSE_ON);

    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();

    if (HSEStartUpStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
    	//FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable);

        /* Flash 2 wait state */
       // FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div2);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd(ENABLE);

        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        /* Select PLL as system clock source */
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);

        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock configuration.
     User can add here some code to deal with this error */

        /* Go to infinite loop */
        while (1)
        {
        }
    }
}
//.............................Set SysTick Definitions.........................................

// SysTick_Handler function will be called every 1 us
void SysTick_Handler()
{
	usTime++;
	if (usTicks != 0)
	{
		usTicks--;
	}
}

void SysTick_Init()
{
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	// Configure the SysTick timer to overflow every 1 us
	SysTick_Config(SystemCoreClock / 1000000);
}

//..............................Set Delays................................

void delayUs(uint32_t us)
{
	// Reload us value
	usTicks = us;
	// Wait until usTick reach zero
	while (usTicks);
}

void delayMs(uint32_t ms)
{
	// Wait until ms reach zero
	while (ms--)
	{
		// delay 1ms
		delayUs(1000);
	}
}

//.............................Set getTime & clear Time functions........................
uint32_t getTime(void){
	usTime_aux = usTime;
	return usTime_aux;
}

void clearTime(void){
	usTime = 0;
}

#endif
