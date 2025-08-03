#include <ti_msp_dl_config.h>
#include "LED.h"
#include "SysTick.h"

extern volatile uint32_t delay_times;

int main(void)
{
	SYSCFG_DL_init();
	SysTick_Init();
	LED_Init();
	while(1)
	{
		DL_GPIO_clearPins(LED_PORT, LED_PIN);
		delay_ms(1000);
		DL_GPIO_setPins(LED_PORT, LED_PIN);
		delay_ms(1000);
	}
}
