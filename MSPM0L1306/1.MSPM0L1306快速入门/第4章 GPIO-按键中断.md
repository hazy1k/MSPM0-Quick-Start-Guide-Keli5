# 第四章 GPIO-按键中断

## 1. 硬件设计

在开发板的原理图中，将PA18引脚接入了一个按键。采用的检测方式是通过给按键的一端接入高电平，一端接入GPIO。在没有按下按键时，因为下拉电阻R8（47KΩ）的存在，导致一直处在低电平；当按键按下时，PA18引脚将被变为高电平的3.3V。这样高低电平的变化，就可以让开发板检测到按键的状态。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/key/key_20240805_110032.png)

## 2. 软件设计

### 2.1 编程大纲

1. 配置KEY的GPIO状态
2. 主函数测试

### 2.2 代码分析

#### 2.2.1 KEY相关参数宏定义

```c
#define KEY_PORT (GPIOA)
#define KEY_PIN (DL_GPIO_PIN_18)
#define KEY_IOMUX (IOMUX_PINCM19)
```

#### 2.2.2 配置GPIO

```c
#include "KEY.h"

void Key_Init(void)
{
    // GPIO状态配置
    DL_GPIO_initDigitalInputFeatures(
        KEY_IOMUX, 
        DL_GPIO_INVERSION_DISABLE, 
        DL_GPIO_RESISTOR_PULL_DOWN, 
        DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE
    );
    DL_GPIO_setUpperPinsPolarity(KEY_PORT, DL_GPIO_PIN_18_EDGE_RISE); // 上升沿检测
}
// DL_GPIO_initDigitalInputFeatures：初始化GPIO输入功能
// KEY_IOMUX：引脚索引
// DL_GPIO_INVERSION_DISABLE：不反转
// DL_GPIO_RESISTOR_PULL_DOWN：下拉
// DL_GPIO_HYSTERESIS_DISABLE：不启用滞后
// DL_GPIO_WAKEUP_DISABLE：不启用唤醒功能
// DL_GPIO_setUpperPinsPolarity：设置组内引脚的极性
// KEY_PORT：引脚组
// DL_GPIO_PIN_18_EDGE_RISE：18号引脚上升沿检测
```

#### 2.2.3 主函数测试

```c
#include <ti_msp_dl_config.h>
#include "LED.h"
#include "KEY.h"

int main(void)
{
	SYSCFG_DL_init();	
	LED_Init();
	Key_Init();
	while(1)
	{
		if(DL_GPIO_readPins(KEY_PORT, KEY_PIN) > 0)
		{
			LED_ON();
		}
		else
		{
			LED_OFF();
		}
	}
}

```
