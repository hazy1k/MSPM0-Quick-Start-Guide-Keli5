#include "ti_msp_dl_config.h"
#include "LED.h"
#include "SYSTICK.h"
#include "TIM.h"

int main(void)
{
	int i = 0;
	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	PWM_LED_init(); 
	while(1) 
	{
		// 模拟呼吸灯
		for(i = 0; i < 999; i++)
		{
			DL_TimerG_setCaptureCompareValue(PWM_TIM, i, PWM_IDX); // 占空比
			delay_ms(1);
		}
		for(i = 999; i > 0; i--)
		{
			DL_TimerG_setCaptureCompareValue(PWM_TIM, i, PWM_IDX);
			delay_ms(1);
		}
	}
}
