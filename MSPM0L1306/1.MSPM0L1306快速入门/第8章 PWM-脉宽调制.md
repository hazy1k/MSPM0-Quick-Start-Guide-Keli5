# 第八章 PWM-脉宽调制

## 1. MSPM0L系列PWM介绍

前面我们介绍了MSPM0L系列一共有4个定时器，可以分为2种类型，通用计时器（TIMG）和高级控制计时器（TIMA）。而PWM功能就是在定时器的基础上实现的。从用户手册上可以了解到，MSPM0L1306有4个定时器，每一个定时器拥有2个PWM通道，每一个PWM通道都对应单片机的一个管脚，这个引脚不是唯一固定的，可能有一个或者两个管脚都对应同一个通道。比如说TIMG_C0对应PA5和PA12，就是说PA5和PA12管脚都可以配置为定时器的通道0，我们在使用的时候可以任选其一进行配置。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/pwm/pwm_20240805_140327.png)

PWM一共有两种模式，一个是边缘对齐PWM，指的是定时器使用的向下计数模式，输出通道在定时器计数开始的时候会输出高电平，**当计数值与输出通道的比较值（图中的CC0和CCP0，CC1和CCP1）相同的时候**，输出通道会输出低电平，一直到定时器的计数值计数到0，就完成了一个周期的脉冲输出。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/pwm/pwm_20240805_140415.png)

另一个模式是中心对齐模式，指的是定时器的上下计数模式，输出通道会在定时器开始的时候输出低电平，在定时器向上计数和向下计数两次到达输出通道的比较值相等的时候各翻转一次电平。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/pwm/pwm_20240805_140422.png)

## 2. 软件设计

### 2.1 编程大纲

1. 定时器相关参数宏定义

2. 定时器配置PWM模式

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 定时器相关宏定义

```c
/* PWM通道(TIMG1)配置 */
#define PWM_TIM TIMG1
#define PWM_TIM_IRQHandler TIMG1_IRQHandler
#define PWM_TIM_IRQn TIMG1_INT_IRQn
#define PWM_TIM_CLK_FREQ 100000
#define PWM_PORT GPIOA
#define PWM_PIN DL_GPIO_PIN_14
#define PWM_IOMUX (IOMUX_PINCM15)
#define PWM_IOMUX_FUNC IOMUX_PINCM15_PF_TIMG1_CCP0
#define PWM_IDX DL_TIMER_CC_0_INDEX
```

#### 2.2.2 定时器工作模式配置

```c
/*
 * Timer clock configuration to be sourced by  / 8 (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   100000 Hz = 4000000 Hz / (8 * (39 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_LEDClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 39U
};

static const DL_TimerG_PWMConfig gPWM_LEDConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN, // 边沿对齐模式
    .period = 1000, // 重载值(999U)：1000ms * 100000Hz - 1 = 999U
    .startTimer = DL_TIMER_START, // 开始->启动
};

void PWM_LED_init(void) 
{
    /* GPIO初始化 */
    DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX, PWM_IOMUX_FUNC);
    DL_GPIO_enableOutput(PWM_PORT, PWM_PIN);
    /* 定时器初始化 */
    DL_TimerG_setClockConfig(PWM_TIM, (DL_TimerG_ClockConfig*)&gPWM_LEDClockConfig); // 时钟配置
    DL_TimerG_initPWMMode(PWM_TIM, (DL_TimerG_PWMConfig*)&gPWM_LEDConfig); // 工作模式配置
    DL_TimerG_setCaptureCompareValue(PWM_TIM, 1000, DL_TIMER_CC_0_INDEX); // 设置捕获比较值
    DL_TimerG_setCaptureCompareOutCtl(PWM_TIM, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX); // 设置捕获比较输出控制
    DL_TimerG_setCaptCompUpdateMethod(PWM_TIM, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_enableClock(PWM_TIM);
    DL_TimerG_setCCPDirection(PWM_TIM, DL_TIMER_CC0_OUTPUT);
}
```

#### 2.2.3 主函数测试

```c
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

```
