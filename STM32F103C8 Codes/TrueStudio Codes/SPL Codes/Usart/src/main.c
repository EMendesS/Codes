#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "string.h"
#include "usart_lib.h"
#include <stdio.h>
#include <stdlib.h>

volatile u16 DataValue = 0;

void Initialize_ADC(void){

	//clock for ADC (max 14MHz --> 72/6=12MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	// enable ADC system clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Initialize the ADC1 according to the ADC_InitStructure members */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);

	/* Configures ADC1 Channel1 as: first converted channel with an 7.5
	cycles sample time */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,
	ADC_SampleTime_7Cycles5);
	/* Configures ADC1 Channel2 as: second converted channel with an 1.5
	cycles sample time */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2,
	ADC_SampleTime_1Cycles5);
	/* Configures ADC1 Channel3 as: third converted channel with an 1.5
	cycles sample time */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3,
	ADC_SampleTime_1Cycles5);

	//ADC calibration (optional, but recommended at power on)
	ADC_ResetCalibration(ADC1);	// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));


}
void Initialize_Peripherics(void)
{
	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef gpioStructure;

    //Config PC13
    gpioStructure.GPIO_Pin = GPIO_Pin_13;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioStructure);

    //Config PA1
    gpioStructure.GPIO_Pin = GPIO_Pin_1;
    gpioStructure.GPIO_Mode = GPIO_Mode_AIN;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);
}

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
        //FLASH_SetLatency( FLASH_Latency_2);

        /* HCLK = SYSCLK */
        RCC_HCLKConfig( RCC_SYSCLK_Div1);

        /* PCLK2 = HCLK */
        RCC_PCLK2Config( RCC_HCLK_Div1);

        /* PCLK1 = HCLK/2 */
        RCC_PCLK1Config( RCC_HCLK_Div2);

        /* PLLCLK = 8MHz * 9 = 72 MHz */
        RCC_PLLConfig(0x00010000, RCC_PLLMul_9);

        /* Enable PLL */
        RCC_PLLCmd( ENABLE);

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

int i=0;

int main(void)
{
	// Set System clock
	SetSysClockTo72();

	GPIO_SetBits(GPIOC, GPIO_Pin_13); // Set C13 to Low level ("0")

	//Initialize ADC
	Initialize_ADC();

    // Initialize USART
    usart_init();
    USARTSend_char(" Hello.\r\nUSART1 is ready.\r\n");

    //INITIALIZE PERIPHERICS
    Initialize_Peripherics();

    // start conversion
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode)

    while (1)
    {
    	for(int i=0; i<1000000; i++){
    		if(i==1){
    			//USARTSend_char("100\n");
    			GPIO_SetBits(GPIOC, GPIO_Pin_13);
    			DataValue = ADC_GetConversionValue(ADC1);
    			USARTSend_int(DataValue);
    			USARTSend_char(" wei\n");
    		}
    	}
    	i=0;
    }
}

