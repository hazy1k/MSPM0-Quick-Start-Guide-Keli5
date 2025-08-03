#include "SYSTICK.h"

volatile uint32_t delay_times = 0;

void SysTick_init(void)
{
    // 系统时钟被设置为32000Hz
    // 延时时间：1ms
    DL_SYSTICK_config(32000);
}

// 延时函数
void delay_ms(uint32_t ms)
{
    delay_times = ms;
    while(delay_times != 0);
}

// 中断服务函数
void SysTick_Handler(void)
{
    if(delay_times != 0)
    {
        delay_times--;
    }
}
