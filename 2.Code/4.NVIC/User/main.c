#include <ti_msp_dl_config.h>
#include "LED.h"
#include "Key.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	KEY_Init();
	NVIC_EnableIRQ(KEY_IRQN); // 使能KEY_IRQN中断
	while(1)
	{
	}
}

// Group1的中断服务函数
void GROUP1_IRQHandler(void)
{ 
	// 读取KEY_IRQN中断的状态
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
	{
		case KEY_INT_IIDX:
			if(DL_GPIO_readPins(KEY_PORT, KEY_PIN) > 0)
			{
				DL_GPIO_togglePins(LED_PORT, LED_PIN);
			}
		break;
	}
}
