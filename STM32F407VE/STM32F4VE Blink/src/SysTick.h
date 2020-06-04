/*
  ******************************************************************************
  * @file		delay.h
  * @author	Yohanes Erwin Setiawan
  * @date		10 January 2016
  ******************************************************************************
  */

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

// For store tick counts in us
static volatile uint32_t usTicks;	// For store tick counts in us
static volatile uint32_t usTime = 0;		//For store time counter in us
static volatile uint32_t usTime_aux = 0;		//For store time counter in us (auxiliar variable)

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
	SysTick_Config(SystemCoreClock / 3000000);//(SystemCoreClock / 3000000);
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
