#include "LED.h"
#include "Timer.h"
#include "SysTick.h"
#include <ti_msp_dl_config.h>

int main(void)
{
    int i;
    SYSCFG_DL_init();
    LED_Init();
		Tim_Init();
    SysTick_Init();
    while(1)
    {
        for(i = 0; i <= 999; i++) // 模拟呼吸灯
        {
            DL_TimerG_setCaptureCompareValue(TIM_PWM, i, PWM_IDX); // 设置PWM占空比
            delay_ms(1);
        }
        for(i = 999; i > 0; i--)
        {
            DL_TimerG_setCaptureCompareValue(TIM_PWM, i, PWM_IDX);
            delay_ms(1);
        }
    }
}
