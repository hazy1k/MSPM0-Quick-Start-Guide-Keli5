#include <ti_msp_dl_config.h>
#include "LED.h"


int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	LED_ON();
	while (1);
}
