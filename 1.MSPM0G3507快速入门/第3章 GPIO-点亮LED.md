# 第三章 点亮LED（GPIO)

## 1. 硬件设计

### 1.1 LED简介

LED 灯（发光二极管）是一种半导体光源，有两种发送接入单片机：

1. 灌入电流

灌入电流指的是LED的供电电流是由外部提供电流，将电流灌入我们的MCU；风险是当外部电源出现变化时，会导致MCU的引脚烧坏。

2. 输出电流

输出电流指的是由MCU提供电压电流，将电流输出给LED；如果使用 MCU的GPIO 直接驱动 LED，则驱动能力较弱，可能无法提供足够的电流驱动 LED。

<img src="https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/led/led_20240626_165507.png" title="" alt="" width="585">

### 1.2 开发板上LED原理图

原理图中，将LED的正极接入开发板的PA14引脚，负极接入限流电阻R17再到GND。通过LED灯的驱动原理可以知道，我们只要控制开发板的PA14引脚输出高电平，即可点亮LED

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/led/led_20240626_165656.png)

## 2. 软件设计

### 2.1 编程大纲

1. MSPM0G3507初始化（参考官方代码）

2. 初始化LED

3. 主函数测试LED闪烁

### 2.2 代码分析

#### 2.2.1 官方系统初始化代码

这个代码是官方自带的-ti_msp_dl_config.c和ti_msp_dl_config.c，用来初始化电源管理和系统控制还有时钟

##### 2.2.1.1 ti_msp_dl_config.h

```c
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X // MCU选择为MSPM0G3507
// 下面的配置是编译器兼容性设置，不必理会
#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

// TI官方的驱动库头文件
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

// C++外部链接声明
/*如果使用 C++ 编译器编译该文件，extern "C" 用于告诉编译器遵循 C 语言的链接规则，
这样 C++ 编译器不会对函数名进行 C++ 特有的名称修饰，从而确保 C 和 C++ 代码可以正常链接。*/
#ifdef __cplusplus
extern "C" {
#endif

// 宏定义电源启动延迟和MCU时钟频率
#define POWER_STARTUP_DELAY (16) // 启动电源后延迟16个时钟周期
#define CPUCLK_FREQ 32000000 // MCU时钟频率为32MHz

// 函数声明
void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_SYSCTL_init(void);

// C++外部链接结束
// 如果使用 C++ 编译器，则结束 extern "C" 块，确保函数按照 C 风格链接。
#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
```

这个文件主要是为了兼容性，具体可以看代码注释，然后可以根据宏定义更改电源启动延时和MCU频率

##### 2.2.1.2 ti_msp_dl_config.c

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
    DL_GPIO_enablePower(GPIOA); // 使能GPIOA的电源
    delay_cycles(POWER_STARTUP_DELAY); // 延时一段时间，等待电源稳定
}

void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统时钟频率-32MHz
}
```

在此我们需要注意两个函数

```c
void SYSCFG_DL_initPower(void)
```

这个函数用来管理GPIO的电流，有点像stm32中的时钟管理，在stm32中想使用一个GPIO，首先得使能时钟

```c
void SYSCFG_DL_SYSCTL_init(void)
```

这个函数用来初始化BOR电压阈值和系统时钟频率，我们默认就行了，学习的话32MHz已经够用了

#### 2.2.2 LED相关参数宏定义

```c
#ifndef __LED_H
#define __LED_H

#include "ti_msp_dl_config.h"

#define LED_PORT GPIOA
#define LED_PIN DL_GPIO_PIN_14
#define LED_IOMUX IOMUX_PINCM36

void LED_Init(void);

#endif 
```

宏定义可以使程序更加清晰，也方便迁移

我们类比STM32发现有一个新的定义，MSPM0G3507 中的 IOMUX 配置

在 **MSPM0G3507** 中，**IOMUX** 配置是通过引脚的选择寄存器进行设置的。通过这些寄存器，用户可以将不同的外设功能映射到各个 I/O 引脚。这些配置涵盖了很多外设功能，比如 GPIO、USART、SPI、PWM、ADC 等等。

类比 STM32 中的 I/O 配置：

在 **STM32** 微控制器中，类似的配置被称为 **GPIO 配置和引脚复用**（Pin Muxing）。STM32 通过每个引脚的 **Alternate Function (AF)** 来实现 I/O 引脚的多路复用。STM32 使用类似的配置机制，通过 **RCC（复位和时钟控制）** 和 **GPIO 配置寄存器** 来完成引脚功能的选择和设置。

#### 2.2.3 LED初始化

```c
#include "LED.h"

void LED_Init(void) // LED初始化
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // 初始化数字输出引脚
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // 清除PA14上的数据
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // 使能PA14输出
}

```

##### 2.2.3.1 DL_GPIO_initDigitalOutput()

- **作用**：该函数的作用是初始化引脚为数字输出模式。
- **参数**：`LED_IOMUX` 是一个宏或常量，表示引脚的 IOMUX 配置。IOMUX（输入输出多路复用）配置决定了引脚的功能。该参数指定了引脚应当被配置为数字输出模式。
- **功能**：在这个步骤中，`DL_GPIO_initDigitalOutput` 配置引脚为数字输出。这意味着该引脚可以用来输出高低电平（0 或 1）。通常，LED 控制引脚就是这样设置的，LED 的开关是通过输出高低电平来控制的。

##### 2.2.3.2 DL_GPIO_clearPins()

- **作用**：该函数的作用是清除（设为低电平）指定的引脚。
- **参数**：
  - `LED_PORT`：通常是一个代表端口的常量或宏，指向包含 LED 控制引脚的 GPIO 端口。例如，`LED_PORT` 可能表示 GPIOA、GPIOB 等。
  - `LED_PIN`：这是一个常量，表示 LED 引脚在该端口中的编号。例如，`LED_PIN` 可能是一个值，代表在 `LED_PORT` 端口中的具体引脚编号（例如 PA14，PB5 等）。
- **功能**：`DL_GPIO_clearPins` 将指定的引脚（在这里是 `LED_PIN`）设置为低电平（0）。这通常意味着关闭 LED，因为 LED 的亮灭是由输出的电平决定的（低电平关闭，高电平打开）。

##### 2.2.3.3 DL_GPIO_enableOutput()

- **作用**：该函数的作用是使能指定引脚作为输出。
- **参数**：
  - `LED_PORT`：指定该引脚所在的端口。
  - `LED_PIN`：指定端口中的具体引脚。
- **功能**：`DL_GPIO_enableOutput` 用于确保指定引脚（`LED_PIN`）处于输出模式，并且能够驱动电平变化。这通常是在配置引脚为输出之后，使其能够控制外部设备（例如 LED）的工作。

### 2.2.4 主函数测试

```c
int main(void)
{
	SYSCFG_DL_init(); // ti_msp_dl_config.c
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

DL_GPIO_setPins()就是指定端口和引脚高电平了

## 3. 小结

本章比较简单，主要就是要认识一下ti_msp_dl_config和几个库函数
