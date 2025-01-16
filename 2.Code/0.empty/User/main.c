#include <ti_msp_dl_config.h>
#include "LED.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	while(1)
	{
		DL_GPIO_setPins(LED_PORT,LED_PIN);
	}
}
