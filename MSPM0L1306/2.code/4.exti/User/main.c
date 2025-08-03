#include <ti_msp_dl_config.h>
#include "LED.h"
#include "KEY.h"

int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	Key_Init();
	NVIC_EnableIRQ(KEY_INT_IRQN); // Enable KEY interrupt
	while(1);
}
