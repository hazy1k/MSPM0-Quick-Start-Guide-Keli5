# 第二章 GPIO-点亮LED

## 1. 硬件设计

LED 驱动指的是通过稳定的电源为 LED 提供合适的电流和电压，使其正常工作点亮。LED 驱动方式主要有恒流和恒压两种。限定电流的恒流驱动是最常见的方式，因为 LED 灯对电流敏感，电流大于其额定值可能导致损坏。恒流驱动保证了稳定的电流，从而确保了 LED 安全。 LED 灯的驱动比较简单，只需要给将对应的正负极接到单片机的正负极即可驱动。LED的接法也分有两种，灌入电流和输出电流。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/led/led_20240805_103609.png)

- 灌入电流指的是LED的供电电流是由外部提供电流，将电流灌入我们的MCU；风险是当外部电源出现变化时，会导致MCU的引脚烧坏。
- 输出电流指的是由MCU提供电压电流，将电流输出给LED；如果使用 MCU的GPIO 直接驱动 LED，则驱动能力较弱，可能无法提供足够的电流驱动 LED。

需要注意的 是 LED 灯的颜色不同，对应的电压也不同。电流不可过大，通常需要接入220欧姆到10K欧姆左右的限流电阻，限流电阻的阻值越大，LED的亮度越暗。

原理图中，将LED的正极接入开发板的PA14引脚，负极接入限流电阻R17再到GND。通过LED灯的驱动原理可以知道，我们只要控制开发板的PA14引脚输出高电平，即可点亮LED；

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/led/led_20240805_103617.png)

## 2. 软件设计

### 2.1 编程大纲

1. 系统初始化

2. 配置GPIO驱动LED

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 系统初始化

```c
#include <ti_msp_dl_config.h>

/* 系统初始化 */
void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_SYSCTL_init();
}

/* GPIO 初始化 */
void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    delay_cycles(POWER_STARTUP_DELAY); // 电源启动延时
}

/* 系统时钟配置 */
void SYSCFG_DL_SYSCTL_init(void)
{
    // 低功耗模式配置为 SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值 
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 使用32MHz作为系统时钟
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE); // 关闭MCLK分频器
}
```

#### 2.2.2 LED驱动宏定义

```c
#include <ti_msp_dl_config.h>

/* LED信息宏定义 */
#define LED_PORT (GPIOA) // 选择GPIOA作为LED端口
#define LED_PIN (DL_GPIO_PIN_14) // 选择GPIOA14作为LED引脚
#define LED_IOMUX (IOMUX_PINCM15) // 复用索引选择

/* LED状态宏定义 */
#define LED_ON() DL_GPIO_setPins(LED_PORT, LED_PIN) // 高电平点亮
#define LED_OFF()  DL_GPIO_clearPins(LED_PORT, LED_PIN) // 低电平熄灭
```

#### 2.2.3 LED初始化

```c
#include "LED.h"

void LED_Init(void)
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // 使能数字引脚输出功能
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // 置低电平
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // 使能输出
}
```

#### 2.2.4 主函数

```c
#include <ti_msp_dl_config.h>
#include "LED.h"

int main(void)
{
    SYSCFG_DL_init();    
    LED_Init();
    LED_ON();
    while (1);
}
```


