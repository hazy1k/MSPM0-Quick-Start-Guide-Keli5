#include <ti_msp_dl_config.h>
#include "LED.h"
#include "KEY.h"

int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	Key_Init();
	while(1)
	{
		if(DL_GPIO_readPins(KEY_PORT, KEY_PIN) > 0)
		{
			LED_ON();
		}
		else
		{
			LED_OFF();
		}
	}
}
