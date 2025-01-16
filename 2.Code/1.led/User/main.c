#include <ti_msp_dl_config.h>
#include "LED.h"

// 软件延时（不精确，后期会改用TickSYS）
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __asm__("nop");
        }
    }
}

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	while(1)
	{
		DL_GPIO_clearPins(LED_PORT, LED_PIN);
		delay_ms(1000);
		DL_GPIO_setPins(LED_PORT, LED_PIN);
		delay_ms(1000);
	}
}
