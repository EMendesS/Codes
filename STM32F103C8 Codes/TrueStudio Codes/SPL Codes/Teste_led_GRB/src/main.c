#include <stddef.h>
#include "stm32f10x.h"
#include "Timer_Led.h"

void InitializeLEDs()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_1;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);

    gpioStructure.GPIO_Pin = GPIO_Pin_13;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioStructure);
}


int main()
{
	SetSysClockTo72();
    InitializeLEDs(); //"Parece" ser boa pratica colocar as inicializacoes dos perifericos no topo da main!

    // Initialize EXTI1
//	TIM2_Interrupt_init(); //Deve vir após "InitializeLEDs()", na main. Do contrario, nao inicializaria corretamente os perifericos

    InitializeTimer();
//  InitializePWMChannel(23); //por 67,22

    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    while(1){
    	if(TIM_GetCounter(TIM2) == 23)
    	GPIO_SetBits(GPIOA, GPIO_Pin_1);
    	if(TIM_GetCounter(TIM2) == 45)
    	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

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
