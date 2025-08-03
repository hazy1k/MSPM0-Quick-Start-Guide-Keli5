# 第七章 TIM-定时器

## 1. 定时器基本参数

### 1.1 预分频

预分频器可以将定时器的时钟(TIMCLK)频率按1到256之间的任意值分频（1到256是基于定时器分频器是8位），TIMG可以选择BUSCLK、MFCLK、LFCLK作为时钟源，可以最大8分频时钟，再经过一个8位的预分频器，最终称为定时器的计数时钟。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/timer/timer_20240805_135226.png)

### 1.2 向上或向下计数模式

向上计数指的是计数器从0开始向上连续计数到自动加载值，一旦计数器计数到自动加载值，会重新从0开始向上计数并产生上溢事件。

向下计数指的是计数器从自动加载值开始向下连续计数到0，一旦计数器计数到0，会重新从自动加载值开始向下计数并产生下溢事件。

### 1.3 更新事件

更新事件是当计数器上溢或者下溢而开始新的计数周期时触发的。更新事件可以触发DMA请求，以用于在下一个计数周期开始时及时更新定时器的运行参数，特别适合用于实时控制。

## 2. 软件设计

### 2.1 编程大纲

1. 相关参数宏定义

2. 定时器工作模式配置

3. 中断服务函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 TIM相关参数宏定义

```c
#define TIMERx (TIMG0)
#define TIMERx_IRQHandler TIMG0_IRQHandler
#define TIMER_IRQn TIMG0_INT_IRQn
#define TIMER_LOAD_VALUE (39999U)
```

#### 2.2.2 TIM工作模式配置

```c
/*
 * Timer clock configuration to be sourced by BUSCLK /  (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000 Hz = 4000000 Hz / (8 * (99 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK, // 时钟源->BUSCLK
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 99U, 
};// 定时器时钟频率被4000000Hz，除以8，再除以100，得到40000Hz
/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_0_INST_LOAD_VALUE = (1000 ms * 40000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIMER_LOAD_VALUE, // 重载值(39999U)：40000Hz * 1000ms - 1 = 39999U
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC, // 定时器模式->周期模式
    .startTimer = DL_TIMER_START, // 开始->启动
};
void TIMER_Init(void) {
    DL_TimerG_setClockConfig(TIMERx,(DL_TimerG_ClockConfig *) &gTIMER_0ClockConfig);
    DL_TimerG_initTimerMode(TIMERx,(DL_TimerG_TimerConfig *) &gTIMER_0TimerConfig);
    DL_TimerG_enableInterrupt(TIMERx, DL_TIMERG_INTERRUPT_ZERO_EVENT); // 0溢出中断
    DL_TimerG_enableClock(TIMERx); // 启动时钟
}
```

#### 2.2.3 中断服务函数

```c
// 中断服务函数
void TIMERx_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(TIMERx))
    {
        case DL_TIMER_IIDX_ZERO: // 如果是0溢出中断
            DL_GPIO_togglePins(LED_PORT, LED_PIN);
            break;
        default:
            break;    
    }
}
```

#### 2.2.4 主函数测试

```c
#include "ti_msp_dl_config.h"
#include "LED.h"
#include "SYSTICK.h"
#include "TIM.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	TIMER_Init();
	NVIC_ClearPendingIRQ(TIMER_IRQn);
	NVIC_EnableIRQ(TIMER_IRQn);
	while (1) 
	{			
	}
}

```
