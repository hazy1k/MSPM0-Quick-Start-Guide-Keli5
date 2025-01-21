# 第九章 TIM-输出PWM

## 1. 硬件设计

### 1.1 PWM简介

WM（Pulse Width Modulation 脉宽调制）是利用微处理器的数字输出来对模拟电路进行控制的一种非常有效的技术。它是一种对模拟信号电平进行数字编码的方法。是指在一定时间内波形的高电平（即 1 状态）所占用的时间比例。通过高分辨率计数器的使用，方波占空比被调制用来对一个模拟信号的电平进行编码。PWM 信号任然是数字的，因为在给定的任何时刻，满幅值的直流供电要么完全有，要么完全无。比如我们的电压输出是 5V的，那么经过改变 PWM 的占空比，可以达到在一定时间内输出 3.3V 或者 1.3V 的效果。

前面我们介绍了MSPM0G系列一共有7个定时器，可以分为2种类型，通用计时器（TIMG）和高级控制计时器（TIMA）。而PWM功能就是在定时器的基础上实现的。从用户手册上可以了解到，MSPM0G3507有7个定时器，哪一个定时器具有PWM功能需要参考数据手册的引脚说明，每一个PWM通道都对应单片机的一个管脚，这个引脚不是唯一固定的，可能有一个或者两个管脚都对应同一个通道。比如说TIMG_C0对应PA5和PA12，就是说PA5和PA12管脚都可以配置为定时器的通道0，我们在使用的时候可以任选其一进行配置。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/pwm/pwm_20240703_154736.png)

### 1.2 PWM基本参数

PWM一共有两种模式，一个是边缘对齐PWM，指的是定时器使用的向下计数模式，输出通道在定时器计数开始的时候会输出高电平， **当计数值与输出通道的比较值（图中的CC0和CCP0，CC1和CCP1）相同的时候，** 输出通道会输出低电平，一直到定时器的计数值计数到0，就完成了一个周期的脉冲输出。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/pwm/pwm_20240703_154837.png)

另一个模式是中心对齐模式，指的是定时器的上下计数模式，输出通道会在定时器开始的时候输出低电平，在定时器向上计数和向下计数两次到达输出通道的比较值相等的时候各翻转一次电平。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/pwm/pwm_20240703_154859.png)

PWM是脉冲宽度调制，具有两个非常重要的参数：频率和占空比。

- 频率：PWM的频率是整个周期的倒数。
- 占空比：占空比是指一个周期内高电平所占的比例。

## 2. 软件设计

### 2.1 编程大纲

1. TIM相关参数宏定义

2. TIM初始化输出PWM

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 TIM相关参数宏定义

```c
#ifndef __TIMER_H
#define __TIMER_H

#include "ti_msp_dl_config.h"

/* TIM相关参数宏定义 */
#define TIM_PWM TIMG12 // 使用的定时器TIMG12
#define TIM_PWM_IRQN (TIMG12_INT_IRQn) // 使用的定时器中断
#define TIM_PWM_IRQHandler TIMG12_IRQHandler // 使用的定时器中断处理函数
#define TIM_PWM_CLKFREQ 4000000 // 定时器时钟频率4MHz

/* TIM输出通道相关参数宏定义 */
#define PWM_PORT GPIOA // 使用的GPIO端口
#define PWM_PIN DL_GPIO_PIN_14 // 使用的GPIO引脚
#define PWM_IOMUX (IOMUX_PINCM36) // 使用的GPIO IOMUX配置
#define PWM_IOMUX_FUNC IOMUX_PINCM36_PF_TIMG12_CCP0 // 使用的GPIO IOMUX功能配置
#define PWM_IDX DL_TIMER_CC_0_INDEX // 使用的定时器输出通道索引

void Tim_Init(void);

#endif /* __TIMER_H */
```

#### 2.2.2 初始化TIM输出PWM

```c
#include "Timer.h"

// TIM时钟配置
static const DL_TimerG_ClockConfig gPWM_LEDClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK, // 时钟源选择为BUSCLK(4MHz)
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8, // 预分频系数为8
    .prescale = 0U, // 预分频值
}; // timerClkFreq = 4000000 Hz / (8 * (0 + 1)) = 4000000 Hz / 8 = 500000 Hz

// TIM工作参数配置
static const DL_TimerG_PWMConfig gPWM_LEDConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN, // PWM模式为边沿对齐
    .period = 1000, // 1000/500000 = 2ms
    .startTimer = DL_TIMER_START, // 启动定时器
};

void Tim_Init(void)
{
    /* GPIO初始化 */
    DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX, PWM_IOMUX_FUNC); // 设置PWM输出功能
    DL_GPIO_enableOutput(PWM_PORT, PWM_PIN); // 使能端口输出
    /* TIM初始化 */
    DL_TimerG_setClockConfig(TIM_PWM, (DL_TimerG_ClockConfig*)&gPWM_LEDClockConfig); // 设置时钟
    DL_TimerG_initPWMMode(TIM_PWM, (DL_TimerG_PWMConfig*)&gPWM_LEDConfig); // 设置PWM工作参数
    /* PWM初始化 */
    // 设置PWM的捕获比较值，此处设置为1000
    DL_TimerG_setCaptureCompareValue(TIM_PWM, 1000, DL_TIMER_CC_0_INDEX);
    // 1.设置初始输出低电平
    // 2.设置输出不反向
    // 3.设置输出源功能的选择
    // 4.设置指定的比较通道
    DL_TimerG_setCaptureCompareOutCtl(TIM_PWM, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
    DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    // 设置捕获比较更新方法为立即更新
    DL_TimerG_setCaptCompUpdateMethod(TIM_PWM, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
    DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_enableClock(TIM_PWM); // 使能定时器时钟
    DL_TimerG_setCCPDirection(TIM_PWM, DL_TIMER_CC0_OUTPUT); // 设置CC0输出方向为正脉冲
}
```

#### 2.2.3 主函数测试

```c
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
```

## 3. 小结

### 3.1 **选择定时器和配置时钟**

MSPM0 微控制器上有多个定时器，可以用来生成 PWM 信号。首先需要选择一个定时器，并确保该定时器的时钟已配置好。

### 3.2**配置定时器为PWM模式**

MSPM0支持多种定时器模式，选择 **PWM模式**，并配置合适的周期（频率）和占空比。

### 3.2 **配置定时器输出引脚**

PWM 信号通常通过微控制器的一个或多个引脚输出。需要配置该引脚为定时器的输出模式。

### 3.4 **启用定时器**

启用定时器的计数功能，开始产生PWM信号。

下面是基于 MSPM0 的代码示例，演示如何配置定时器并生成PWM信号：

```c
#include "mspm0.h"

// 假设我们选择定时器 TMR0，并配置为 PWM 输出
void MSPM0_PWM_Init(void)
{
    // 1. 启用定时器 TMR0 时钟（具体时钟源取决于 MCU 配置）
    __HAL_RCC_TMR0_CLK_ENABLE();
    // 2. 配置定时器模式为 PWM 模式
    TMR0->CTL &= ~TMR_CTL_MODE;              // 清除模式位
    TMR0->CTL |= TMR_CTL_MODE_PWM;           // 设置为 PWM 模式
    // 3. 配置定时器周期（比如设置为 1000个计数周期，具体根据时钟频率）
    TMR0->ARR = 1000 - 1;  // 自动重载值
    // 4. 设置 PWM 占空比（例如设置为 50% 占空比）
    TMR0->CCR = 500;       // 捕获比较寄存器的值决定占空比
    // 5. 配置定时器输出引脚为定时器的 PWM 输出
    // 假设我们用 GPIOA 引脚 0 输出 PWM 信号
    GPIOA->MODER &= ~GPIO_MODER_MODE0;   // 清除原有模式
    GPIOA->MODER |= GPIO_MODER_MODE0_1;  // 设置为复用功能
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1; // 设置为定时器 TMR0 的输出
    // 6. 启动定时器
    TMR0->CTL |= TMR_CTL_CEN;  // 启动计时器
    // 7. 启用定时器的 PWM 输出
    TMR0->CCER |= TMR_CCER_CC0E;   // 启用通道0的输出
}

int main(void)
{
    // 初始化 PWM
    MSPM0_PWM_Init();

    // 主循环
    while (1)
    {
        // 在此循环中，PWM 信号会根据初始化设置继续输出
    }
}
```
