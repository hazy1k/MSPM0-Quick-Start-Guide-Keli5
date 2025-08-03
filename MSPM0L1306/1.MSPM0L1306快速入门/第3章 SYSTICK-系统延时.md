# 第三章 SYSTICK-系统延时

## 1. 滴答定时器介绍

SysTick定时器可用作标准的下行计数器，是一个24位向下计数器，有自动重新装载能力，可屏蔽系统中断发生器。Cortex-M0处理器内部包含了一个简单的定时器，所有基于M0内核的控制器都带有SysTick定时器,这样就方便了程序在不同的器件之间的移植。SysTick定时器可用于操作系统，提供系统必要的时钟节拍，为操作系统的任务调度提供一个有节奏的“心跳”。正因为所有的M0内核的芯片都有Systick定时器，这在移植的时候不像普通定时器那样难以移植。

  RCU 通过 MCLK 作为 Cortex 系统定时器（SysTick）的外部时钟，即使用MCLK计时，MCLK默认为32MHz。通过对 SysTick 控制和状态寄存器的设置，即可控制或读取。关于系统时钟的介绍可参考用户手册的第128页。

  SysTick定时器设定初值并使能之后，每经过1个系统时钟周期，计数值就减1，减到0时，SysTick计数器自动重新装载初值并继续计数，同时内部的COUNTFLAG标志位被置位，触发中断（前提开启中断）。

## 2. 软件设计

### 2.1 编程大纲

1. SYSTICK配置

2. 主函数测试

### 2.2 代码分析

#### 2.2.1 SYSTICK配置

```c
void SysTick_init(void)
{
    // 系统时钟被设置为32000Hz
    // 延时时间：1ms
    DL_SYSTICK_config(32000);
}
```

#### 2.2.2 中断服务函数和延时函数

```c
volatile uint32_t delay_times = 0;
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
```

#### 2.2.3 主函数测试

```c
#include <ti_msp_dl_config.h>
#include "LED.h"
#include "SYSTICK.h"

int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	SysTick_init();
	while(1)
	{
		LED_ON();
		delay_ms(1000);
		LED_OFF();
		delay_ms(1000);
	}
}

```
