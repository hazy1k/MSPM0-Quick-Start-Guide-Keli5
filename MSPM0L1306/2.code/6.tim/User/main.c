#include "ti_msp_dl_config.h"
#include "LED.h"
#include "SYSTICK.h"
#include "TIM.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	TIMER_Init();
	NVIC_ClearPendingIRQ(TIMER_IRQn);
	NVIC_EnableIRQ(TIMER_IRQn);
	while (1) 
	{			
	}
}
