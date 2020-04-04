#include <stddef.h>
#include "stm32f10x.h"

volatile int counter_cycle = 0;

void InitializeTimer()
{
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 100;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &timerInitStructure);
}

void InitializePWMChannel(int pulse1)
{

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = pulse1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);	//SELECIONA CANAL2 DO TIMER 2 (PA1, NESSE CASO)

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void InitializeLEDs()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_6;
    gpioStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioStructure);

    gpioStructure.GPIO_Pin = GPIO_Pin_15;
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &gpioStructure);
}



void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET){
	    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
	    //TIM3->CCR1 = 10;
	}
    counter_cycle++;

    switch(counter_cycle){


    case 1:
    	TIM3->CCR1 = 73;	//VERDE = ESSE BIT "75"
    	break;

    case 2:
    	TIM3->CCR1 = 25;
    	break;

    case 3:
    	TIM3->CCR1 = 25;	//VERMELHOR = ESSE BIT "75"
    	break;

    case 4:
    	TIM3->CCR1 = 25;

    case 5:
    	TIM3->CCR1 = 25;	//AZUL = ESSE BIT "75"
    	break;

    case 6:
    	TIM3->CCR1 = 25;
    	break;

    case 7:
    	TIM3->CCR1 = 25;
    	break;

    case 8:
    	TIM3->CCR1 = 25;
    	break;

    case 9:
    	TIM3->CCR1 = 0;
    	break;

    case 72:
    	counter_cycle = 0;
    	TIM3->CCR1 = 25;
    	break;
    }
}

void TIM3_Interrupt_init(void)
{
	/* NVIC Configuration */
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//TIM_SelectInputTrigger(TIM3, TIM_TS_ITR0); //SELECIONA O SINAL COMO TRIG DO TIM3
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); //ATIVA A FONTE DE INTERRUPT: TIM3, CANAL 2
}

int main()
{
    InitializeLEDs(); //"Parece" ser boa pratica colocar as inicializacoes dos perifericos no topo da main!

    // Initialize EXTI1
	TIM3_Interrupt_init(); //Deve vir após "InitializeLEDs()", na main. Do contrario, nao inicializaria corretamente os perifericos

    InitializeTimer();
    InitializePWMChannel(75); //75,22

    GPIO_ResetBits(GPIOC, GPIO_Pin_15);

    while(1){
        /*if(counter_cycle == 0){
        	TIM3->CCR1 = 25;
        }
        if(counter_cycle == 1){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 2){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 3){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 4){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 5){
            TIM3->CCR1 = 75;
        }
        if(counter_cycle == 6){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 7){
            TIM3->CCR1 = 25;
        }
        if(counter_cycle == 8){
            TIM3->CCR1 = 0;
        }*/
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
