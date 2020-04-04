/* Includes */
#include <stddef.h>
#include "stm32f10x.h"
#include <stdlib.h>

volatile int numero = 0;
char palavra[4];

void Initialize_Timer()
{
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 99;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1000;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
}

void Initialize_PWM_Output(int pulse1)
{

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = pulse1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_Pulse = pulse1/2;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);	//SELECIONA CANAL4 DO TIMER 2 (PA3, NESSE CASO)

    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void Initialize_Input()
{
	//Initialize the TIM according to the following settings:
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; //SELECIONA O CANAL 2 (ISTO É, TI2)
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //ESCOLHER A POLARIDADE DA BORDA A SER DETECTADA PELO TIM2.
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //MAPEIA O CCR2 NO CANAL 2 (TI2) - (selecionar TIM_ICSelection_IndirectTI significa que o CCR1 seria mapeado em TI2)
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //PRESCALER DO TIMER
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	//Configures the TIM peripheral in PWM Input mode according to the parameters specified in the TIM_ICInitStruct
	TIM_PWMIConfig(TIM2, &TIM_ICInitStructure); //INPLICITAMENTE, O CCR1 JA ESTA MAPEADO EM TI2, DEVIDO À PROPRIA FUNCAO. EH O CCR1 QUE DA O DUTY-CYCLE
}
//OBS.: O CCR2 DARÁ O PERIODO E O CCR1, O DUTY.
void Initialize_Peripherics()
{
	/* GPIOA clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* GPIOC clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//Config PA2 e PA3
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);

    //Config PC13
    gpioStructure.GPIO_Pin = GPIO_Pin_13;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioStructure);

    //Config PA1
    gpioStructure.GPIO_Pin = GPIO_Pin_1;
    gpioStructure.GPIO_Mode = GPIO_Mode_IPD;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);
}

void Config_Mode_Input(){
	//TIM_TIxExternalClockConfig(TIM2, TIM_TS_TI2FP2,TIM_ICPolarity_Rising, 0);
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2); //SELECIONA O SINAL TI2FP2 COMO TRIG DO TIM2
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE); //ATIVA A FONTE DE INTERRUPT: TIM2, CANAL 2

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset); //reset the counter on rising edge of the trigger
	TIM_SelectMasterSlaveMode(TIM2,TIM_MasterSlaveMode_Enable);
}

void Interrupt_Configuration(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    NVIC_InitTypeDef NVIC_InitStructure;

    // Enable the TIM2 global Interrupt
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //CONFIGURAR "RISING" E "IPD" NO PINO GARANTEM QUE EM CASO DE MAL CONTATO, O DUTY-CYCLE NAO PASSARÁ DO VALOR DO DUTY DO PWM DE ENTRADA (QUESTAO DE SEGURANCA)
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void EXTI1_IRQHandler(void){

	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
		{
		    EXTI_ClearITPendingBit(EXTI_Line1);
		    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		    TIM2->CCR3 = TIM2->CCR1;
		    //numero = 200;
		    //itoa(numero, palavra, 10);
		       //USARTSend(palavra);
		       //USARTSend("\n");
		}
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

int main(void)
{

	Initialize_Timer();
	Initialize_PWM_Output(300);
	Initialize_Input();
	Initialize_Peripherics();
	Config_Mode_Input();
	Interrupt_Configuration();

//	EXTI_Config();

  while (1)
  {


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
