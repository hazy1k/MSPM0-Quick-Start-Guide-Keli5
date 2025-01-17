#include <ti_msp_dl_config.h>
#include "LED.h"
#include "Key.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	KEY_Init();
	while(1)
	{
		if(DL_GPIO_readPins(KEY_PROT, KEY_PIN) > 0) // 按键按下
		{
			DL_GPIO_setPins(LED_PORT, LED_PIN);
		}
		else
		{
			DL_GPIO_clearPins(LED_PORT, LED_PIN);
		}
	}
}
// DL_GPIO_readPins()库函数用来读取指定GPIO端口的指定引脚的电平状态。
