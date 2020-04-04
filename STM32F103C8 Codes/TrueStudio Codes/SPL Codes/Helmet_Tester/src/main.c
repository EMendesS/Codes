#include <stddef.h>
#include "stm32f10x.h"
#include "SysTick.h"

int i=0;

void InitializeTimer()
{
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 99;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 100;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
}

void InitializePWMChannel(int pulse1, int pulse2)
{

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = pulse1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);	//SELECIONA CANAL3 DO TIMER 1 (PA2, NESSE CASO)

    TIM_OCInitStructure.TIM_Pulse = pulse2;
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);	//SELECIONA CANAL4 DO TIMER 1 (PA3, NESSE CASO)

    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void InitializeLEDs()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);

}

int main()
{
    InitializeLEDs();
    InitializeTimer();
    InitializePWMChannel(70,10);
    SysTick_Init();

    while(1){

    	TIM2->ARR = 1000;
    	delayMs(50);
    	TIM2->ARR = 500;
    	delayMs(50);
    	TIM2->ARR = 2000;
    	delayMs(50);

    	delayMs(300);

    	//for(int i=0 ; i<10000000 ; i++){
    		//TIM2->CCR4 = (50);
    		//TIM2->CCR3 = (50);
    		//TIM2->CCR3 = (int)(i/100000);
    	    //TIM2->CCR4 = (int)(100 - (i/100000));
    	//}
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
