#include "SysTick.h"

volatile uint32_t delay_times = 0;

void SysTick_Init(void)
{
    DL_SYSTICK_config(32000); // 系统32kHz，延时1ms
}

// 利用SysTick定时器实现ms级延时
void delay_ms(uint32_t ms)
{
    delay_times = ms;
    while(delay_times != 0);
}

// SysTick中断服务函数
void SysTick_Handler(void)
{
    __disable_irq(); // 关闭中断
    if(delay_times != 0)
    {
        delay_times--;
    }
    __enable_irq(); // 开启中断
}