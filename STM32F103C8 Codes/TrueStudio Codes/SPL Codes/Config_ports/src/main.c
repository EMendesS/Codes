#include <stddef.h>
#include "stm32f10x.h"
#include "stm32f1xx_hal.h"

void Initialize(){
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_Speed_50MHz;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main()
{
    InitializeLEDs();
    InitializeTimer();
    InitializePWMChannel(400,800);
    InitializeInput();
    InitializeInterrupt();

    while(1){
    }
    	void TIM3_IRQHandler(void)
    	{
    	  /* Clear TIM2 Capture compare interrupt pending bit */
    	  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

    	  /* Get the Input Capture value */
    	  u16 ICValue = TIM_GetCapture1(TIM3);
    	  TIM2->CCR4 = 0;
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
