# 第五章 GPIO-按键控制

## 1. 硬件设计

### 1.1 按键简介

独立按键是一种简单的输入设备，广泛应用于各种电子设备中，用于实现基本的用户交互。它们的工作原理通常基于一个简单的机械开关，当按下按键时触发某些操作。独立按键可以有多种尺寸、形状和颜色，便于用户辨识和使用。

独立按键的主要结构组成包括：按钮、外壳、弹簧、触点、导电片和引脚。当按下按键时，导电片触碰到触点，从而形成一个闭合电路。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/key/key_20240626_180258.png)

独立按键原理主要是基于机械触点和电气触点之间的关系。当按键未被按下时，通常触点是分开的，电路是断开的。当用户按下按键时，触点在弹簧和导电片的作用下形成闭合，这时电路连通，微控制器能够读取到该按键触发的信号。

独立按键驱动是为了让微控制器能识别按键的状态，而微控制器正好可以识别高电平和低电平，所以大多数的按键都是通过给按键的一端接入高电平，一端接入GPIO；或者是给按键的一端接入低电平，一端接入GPIO。通过检测连接按键的引脚有没有发生电平变化，就可以知道按键是否按下。

### 1.2 开发板上按键原理图

在开发板的原理图中，将PA18引脚接入了一个按键。采用的检测方式是通过给按键的一端接入高电平，一端接入GPIO。在没有按下按键时，因为下拉电阻R8（47KΩ）的存在，导致一直处在低电平；当按键按下时，PA18引脚将被变为高电平的3.3V。这样高低电平的变化，就可以让开发板检测到按键的状态。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/key/key_20240626_180543.png)

## 2. 软件设计

### 2.1 编程大纲

1. 初始化ti_msp_dl_config文件

2. 初始化key

3. 主函数测试

### 2.2 代码分析

#### 2.2.1 配置ti_msp_dl_config

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

    DL_SYSCTL_disableHFXT(); // 关闭HFXT
    DL_SYSCTL_disableSYSPLL(); // 关闭系统PLL
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1); // 设置ULP时钟分频系数为1
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE); // 关闭MCLK分频器
}
```

 `DL_SYSCTL_disableHFXT();`

- **功能**：关闭 **HFXT（高频晶体振荡器）**。
- **解释**：HFXT 是系统中的一个外部高频晶体振荡器，它通常用于提供更高精度的时钟源。如果该时钟源不需要或在低功耗模式下不使用，可以将其禁用。
- **作用**：关闭 HFXT 可以降低功耗，减少系统的复杂度，尤其是在不需要高频时钟时。

`DL_SYSCTL_disableSYSPLL();`

- **功能**：关闭 **系统PLL（Phase Locked Loop，锁相环）**。
- **解释**：系统PLL 是用来产生较高频率时钟源的电路。它通常用于倍频，从而提供更高的系统时钟。如果系统不需要高频时钟或希望进入低功耗模式，可以禁用它。
- **作用**：关闭系统 PLL 减少了高频时钟的生成，进一步降低了功耗。

`DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);`

- **功能**：设置 **ULP（Ultra Low Power，超低功耗）时钟分频器** 的系数为 1。
- **解释**：ULP 时钟用于系统进入低功耗模式时提供时钟源。`DL_SYSCTL_ULPCLK_DIV_1` 表示不分频，即保持时钟频率为基准频率。这通常用于在低功耗模式下确保时钟仍然可用。
- **作用**：确保超低功耗时钟（ULP）正常工作并不降低时钟频率。

 `DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);`

- **功能**：关闭 **MCLK（Main Clock，主时钟）分频器**。
- **解释**：MCLK 是主系统时钟源的分频器，通过该设置可以控制 MCLK 的频率。`DL_SYSCTL_MCLK_DIVIDER_DISABLE` 表示禁用主时钟分频器，即不对主时钟进行分频，主时钟频率保持原样。
- **作用**：通过禁用 MCLK 分频器，确保主时钟信号以全频率运行。

#### 2.2.2 Key初始化配置

```c
#include "Key.h"

void KEY_Init(void)
{
    DL_GPIO_initDigitalInputFeatures(KEY_IOMUX, DL_GPIO_INVERSION_DISABLE, 
    DL_GPIO_RESISTOR_PULL_DOWN,DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
}
```

- **参数**：
  
  - `KEY_IOMUX`：表示引脚 18 用于输入（按键）。`IOMUX` 配置指示该引脚作为数字输入。
  - `DL_GPIO_INVERSION_DISABLE`：禁用输入信号反转，保持输入信号的原始状态。
  - `DL_GPIO_RESISTOR_PULL_DOWN`：启用下拉电阻，当该引脚没有输入信号时，会被拉低电平（地电平）。
  - `DL_GPIO_HYSTERESIS_DISABLE`：禁用输入引脚的滞后效应。滞后通常用于减少噪声对信号的影响，但这里禁用了滞后功能。
  - `DL_GPIO_WAKEUP_DISABLE`：禁用该引脚的唤醒功能，即该引脚不会触发系统从低功耗模式中恢复。

##### 2.2.2.1 initDigitalInputFeatures()函数介绍

这个库函数 `DL_GPIO_initDigitalInputFeatures` 用于配置一个引脚作为 GPIO 输入，并可以设置该引脚的一些特性，比如反转、内部电阻、滞后和唤醒行为。该函数通过修改特定的寄存器来实现这些配置。以下是对每个参数和函数的详细解释，以及如何使用这个库函数。

```c
__STATIC_INLINE void DL_GPIO_initDigitalInputFeatures(uint32_t pincmIndex,
    DL_GPIO_INVERSION inversion, DL_GPIO_RESISTOR internalResistor,
    DL_GPIO_HYSTERESIS hysteresis, DL_GPIO_WAKEUP wakeup)
```

- **pincmIndex**: 目标引脚对应的 `PINCM` 寄存器索引。每个引脚在硬件中都有一个唯一的索引，`pincmIndex` 用于指示要配置的具体引脚。
- **inversion**: 配置该引脚的输入信号是否需要反转。可能的取值是 `DL_GPIO_INVERSION` 枚举类型中的一个值。
- **internalResistor**: 配置该引脚的内部电阻类型。可能的取值是 `DL_GPIO_RESISTOR` 枚举类型中的一个值，如上拉电阻、下拉电阻或无电阻。
- **hysteresis**: 配置是否启用该引脚的滞后效应。可能的取值是 `DL_GPIO_HYSTERESIS` 枚举类型中的一个值，用于减少输入噪声。
- **wakeup**: 配置该引脚的唤醒行为。可能的取值是 `DL_GPIO_WAKEUP` 枚举类型中的一个值，控制该引脚是否能够从低功耗模式中唤醒系统。

```c
void KEY_Init(void)
{
    uint32_t pincmIndex = KEY_IOMUX; // 目标引脚
    DL_GPIO_INVERSION inversion = DL_GPIO_INVERSION_DISABLE; // 设置为不反转输入
    DL_GPIO_RESISTOR internalResistor = DL_GPIO_RESISTOR_PULL_DOWN; // 设置为下拉电阻
    DL_GPIO_HYSTERESIS hysteresis = DL_GPIO_HYSTERESIS_DISABLE; // 禁用滞后
    DL_GPIO_WAKEUP wakeup = DL_GPIO_WAKEUP_DISABLE; // 禁用唤醒功能
    DL_GPIO_initDigitalInputFeatures(pincmIndex, inversion, internalResistor, hysteresis, wakeup);
}
```

1. 提供目标引脚的索引（`pincmIndex`）。
2. 配置反转、内部电阻、滞后和唤醒等输入特性。
3. 调用该函数来应用这些配置。

#### 2.2.3 主函数测试

```c
#include <ti_msp_dl_config.h>
#include "LED.h"
#include "Key.h"

int main(void)
{
    SYSCFG_DL_init();
    LED_Init();
    KEY_Init();
    while(1)
    {
        if(DL_GPIO_readPins(KEY_PROT, KEY_PIN) > 0) // 按键按下
        {
            DL_GPIO_setPins(LED_PORT, LED_PIN);
        }
        else
        {
            DL_GPIO_clearPins(LED_PORT, LED_PIN);
        }
    }
}
// DL_GPIO_readPins()库函数用来读取指定GPIO端口的指定引脚的电平状态。
```

## 3. 小结

这一章就主要是学习DL_GPIO_initDigitalInputFeatures()这个库函数了，用来配置引脚为输出状态，具体讲解看前文；还有一个DL_GPIO_readPins()用来读取指定引脚电平状态。

理解了这两个函数，其余的就没问题了
