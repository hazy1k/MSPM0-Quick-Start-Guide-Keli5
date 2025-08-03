# 第八章 TIM-定时功能

## 1. 硬件设计

### 1.1 定时器简介

定时器是单片机内部集成，可以通过编程控制。单片机的定时功能是通过计数来实现的，当单片机每一个机器周期产生一个脉冲时，计数器就加一。定时器的主要功能是用来计时，时间到达之后可以产生中断，提醒计时时间到，然后可以在中断函数中去执行功能。比如我们想让一个led灯1秒钟翻转一次，就可以使用定时器配置为1秒钟触发中断，然后在中断函数中执行led翻转的程序。

#### 1.1.1 MSPM0G系列定时器

MSPM0G系列一共有7个定时器，可以分为2种类型，通用计时器（TIMG）和高级控制计时器（TIMA）。不同类型的定时器所拥有的功能数量不同，一般高级定时器的功能最多，通用定时器次之。具体功能对照可以查看用户手册的第1436页。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/timer/timer_20240703_153426.png)

#### 1.1.2 高级控制定时器

TIMA模块由一个16位自动重载计数器组成，由可编程预调节器驱动，最多四个捕获/比较（CC）块用于多个捕获/比较，带死区插入的PWM输出和间隔定时。TIMA具有广泛的事件生成功能，它们来自不同的计数器事件，如溢出、重载，以及来自每个捕获/比较寄存器。它还具有硬件设计来处理由内部或外部电路产生的故障信号，以指示系统中的故障。

#### 1.1.3 通用定时器

TIMG模块由16位和32位自动重新加载计数器组成，由可编程预计算器驱动，具有两个捕获/比较（CC）块，用于多个捕获/比较、PWM输出和间隔定时。TIMG还具有广泛的事件生成功能，包括计数器溢出、重加载和针对各种用例的捕获/比较操作。

### 1.2 定时器基本参数

#### 1.2.1 预分频

预分频器可以将定时器的时钟(TIMCLK)频率按1到256之间的任意值分频（1到256是基于定时器分频器是8位），TIMG可以选择BUSCLK、MFCLK、LFCLK作为时钟源，可以最大8分频时钟，再经过一个8位的预分频器，最终称为定时器的计数时钟。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/timer/timer_20240703_153435.png)

#### 1.2.2 向上或向下计数模式

向上计数指的是计数器从0开始向上连续计数到自动加载值，一旦计数器计数到自动加载值，会重新从0开始向上计数并产生上溢事件。 向下计数指的是计数器从自动加载值开始向下连续计数到0，一旦计数器计数到0，会重新从自动加载值开始向下计数并产生下溢事件。

#### 1.2.3 更新事件

更新事件是当计数器上溢或者下溢而开始新的计数周期时触发的。更新事件可以触发DMA请求，以用于在下一个计数周期开始时及时更新定时器的运行参数，特别适合用于实时控制。

## 2. 软件设计

### 2.1 编程大纲

1. ti_msp_dl_config文件配置

2. TIM相关参数宏定义

3. 定时器配置，定时器初始化

4. 中断服务函数

5. 主函数测试

### 2.2 代码分析

#### 2.2.2.1 ti_msp_dl_config文件配置

```c
#include "ti_msp_dl_config.h"

void SYSCFG_DL_init(void) // 系统初始化
{
    SYSCFG_DL_initPower(); // 电源初始化
    SYSCFG_DL_SYSCTL_init(); // 系统时钟初始化
}

void SYSCFG_DL_initPower(void) // 电源初始化
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_reset(GPIOB); // 复位GPIOB
    DL_UART_Main_reset(UART0); // 复位UART0
    DL_TimerG_reset(TIMG0); // 复位TimerG0

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB
    DL_UART_Main_enablePower(UART0); // 使能UART0
    DL_TimerG_enablePower(TIMG0); // 使能TimerG0

    delay_cycles(POWER_STARTUP_DELAY); // 等待启动时间
}

void SYSCFG_DL_SYSCTL_init(void) // 系统时钟初始化
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统频率为32MHz
	DL_SYSCTL_disableHFXT(); // 关闭HFXT
	DL_SYSCTL_disableSYSPLL(); // 关闭系统PLL
    DL_SYSCTL_enableMFCLK(); // 使能主时钟-MFCLK
}
/*
UART有三个时钟源：
1.BUSCLK:内部高速振荡器提供，出厂设置为32MHz
2.LFCLK:内部低速振荡器提供，低功耗时有效（32KHz）
3.MFCLK:固定的4MHz时钟
*/
```

#### 2.2.2 TIM相关参数宏定义

```c
#ifndef __TIMER_H
#define __TIMER_H

#include "ti_msp_dl_config.h"

/* TIM相关参数宏定义 */
#define TIMx (TIMG0) // TIMx 选择
#define TIM_IRQN (TIMG0_INT_IRQn) // TIMx 中断向量号
#define TIM_IRQHandler TIMG0_IRQHandler // TIMx 中断处理函数
#define TIM_LOAD_VAL (39999U) // 定时器计数值

void Tim_Init(void);

#endif /* __TIMER_H */

```

#### 2.2.3 TIM初始化

```c
#include "Timer.h"
#include "LED.h"
// TIM时钟配置
/* 具体计算过程
1. 首先确定时钟源，这里选择BUSCLK时钟（4MHz）
2. 确定分频系数，这里选择8分频，确定预分频值，这里选择99
3. 定时器频率 = 时钟源/(预分频值+1)分频系数 = 4000000/(99=1)8 = 40000Hz = 40KHz
*/
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK, // 时钟源选择，这里选择BUSCLK时钟
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8, // 时钟分频系数，这里选择8分频
    .prescale    = 99U, // 预分频值，这里选择99
};    

// TIM工作参数配置
/* 具体计算过程
1. 定时器的时钟频率已经配置为40KHz
2. 定时器的重载值 = (计数周期*定时器频率)-1 = 39999
3. 所以计数周期 = (3999+1)/ 40000 = 1s 
4. 即定时器每秒产生中断一次
*/
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIM_LOAD_VAL, // 定时器重载值，宏定义为39999U
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC, // 定时器工作模式，这里选择周期模式
    .startTimer = DL_TIMER_START, // 启动定时器，这里选择启动
};

// 初始化定时器
void Tim_Init(void)
{
    DL_TimerG_setClockConfig(TIMx, (DL_TimerG_ClockConfig*)&gTIMER_0ClockConfig); // 设置时钟配置
    DL_TimerG_initTimerMode(TIMx, (DL_TimerG_TimerConfig*)&gTIMER_0TimerConfig); // 初始化定时器工作参数
    DL_TimerG_enableInterrupt(TIMx, DL_TIMERG_INTERRUPT_ZERO_EVENT); // 使能定时器0中断，0溢出中断
    DL_TimerG_enableClock(TIMx); // 使能定时器
}
```

#### 2.2.4 TIM中断服务函数

```c
void TIM_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(TIMx)) 
    {
        case DL_TIMERG_IIDX_ZERO: 
            DL_GPIO_togglePins(LED_PORT, LED_PIN);
            break;
        default:
            break;
    }
}
```

#### 2.2.5 主函数

```c
#include "LED.h"
#include "Timer.h"
#include <ti_msp_dl_config.h>

int main(void)
{
    SYSCFG_DL_init();
    LED_Init();
    Tim_Init();
    NVIC_ClearPendingIRQ(TIM_IRQN); // clear any pending interrupt
    NVIC_EnableIRQ(TIM_IRQN); // enable interrupt
    while(1)
    {
    }
}
```

## 3. 小结

定时器的基本工作原理

- **定时器时钟源**：MSPM0 定时器通常由系统时钟（如 BUSCLK）驱动，通过设置分频器和预分频器来控制定时器的频率。
- **定时器的模式**：定时器通常可以工作在不同的模式下，最常见的是周期模式（`Periodic Mode`），定时器在每个计数周期结束时产生一个中断。
