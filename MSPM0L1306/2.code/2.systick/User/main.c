#include <ti_msp_dl_config.h>
#include "LED.h"
#include "SYSTICK.h"

int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	SysTick_init();
	while(1)
	{
		LED_ON();
		delay_ms(1000);
		LED_OFF();
		delay_ms(1000);
	}
}
