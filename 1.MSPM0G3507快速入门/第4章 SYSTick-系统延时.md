# 第四章 SYSTick-系统延时

## 1. 硬件设计

SysTick定时器可用作标准的下行计数器，是一个24位向下计数器，有自动重新装载能力，可屏蔽系统中断发生器。Cortex-M0处理器内部包含了一个简单的定时器，所有基于M0内核的控制器都带有SysTick定时器,这样就方便了程序在不同的器件之间的移植。SysTick定时器可用于操作系统，提供系统必要的时钟节拍，为操作系统的任务调度提供一个有节奏的“心跳”。正因为所有的M0内核的芯片都有Systick定时器，这在移植的时候不像普通定时器那样难以移植。

SysTick定时器设定初值并使能之后，每经过1个系统时钟周期，计数值就减1，减到0时，SysTick计数器自动重新装载初值并继续计数，同时内部的COUNTFLAG标志位被置位，触发中断（前提开启中断）。

## 2. 软件设计

### 2.1 编程大纲

1. 配置ti_msp_dl_config文件初始化

2. 配置SYSTick初始化，利用SysTick实现ms级延时函数

3. 编写SYSTick中断服务函数

4. 主函数测试LED闪烁

### 2.2 代码分析

#### 2.2.1 ti_msp_dl_config文件初始化

```c
#include <ti_msp_dl_config.h>

// 总系统初始化
void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower(); // 初始化电源管理
    SYSCFG_DL_SYSCTL_init(); // 初始化系统控制，配置时钟和电源管理
}

// 电源管理初始化
void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_reset(GPIOB); // 复位GPIOB

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA的电源
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB的电源

    delay_cycles(POWER_STARTUP_DELAY); // 延时一段时间，等待电源稳定
}

void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统时钟频率-32MHz
}

```

#### 2.2.2 SysTick初始化，实现延时函数

```c
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
```

#### 2.2.3 SysTick中断服务函数

```c
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
```

1. **定时减少延时计数**：每当 `SysTick` 定时器发生溢出时，这个中断服务函数就会被调用。函数检查 `delay_times` 变量，并在其不为零时减一。
2. **延时管理**：通过减少 `delay_times`，可以实现基于 `SysTick` 的延时机制。例如，当 `delay_times` 被设置为一个正值时，这个计数器就会随着每次中断而减少，直到其值为零为止。

#### 2.2.4 LED闪烁测试

```c
#include <ti_msp_dl_config.h>
#include "LED.h"
#include "SysTick.h"

extern volatile uint32_t delay_times;

int main(void)
{
	SYSCFG_DL_init();
	SysTick_Init();
	LED_Init();
	while(1)
	{
		DL_GPIO_clearPins(LED_PORT, LED_PIN);
		delay_ms(1000);
		DL_GPIO_setPins(LED_PORT, LED_PIN);
		delay_ms(1000);
	}
}

```

## 3. 小结

这一章依旧比较简单，只比上一章多了一个SysTick，SysTick在TI中还算比较好配置，在STM32中还需要配置时钟树还有EXTI和NVIC，在本章，只需要一个库函数就行了：

```c
DL_SYSTICK_comfig(); // 函数参数：时钟频率
```

这个函数其实来自M0内核：

```c
/**
 *  @brief Redirects DL_SYSTICK_config to CMSIS standard SysTick_Config
 */
#define DL_SYSTICK_config(x)                                   SysTick_Config(x)
```

```c
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
}
```


