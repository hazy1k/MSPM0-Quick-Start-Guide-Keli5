# 第五章 EXTI-外部中断

## 1. 硬件设计

## 2. 软件设计

### 2.1 编程大纲

1. 中断配置

2. 主函数测试

### 2.2 代码分析

#### 2.2.1 相关参数宏定义

```c
  /* 中断相关宏定义*/
#define KEY_INT_IRQN (GPIOA_INT_IRQn)
#define KEY_INT_IIDX (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
```

#### 2.2.2 中断配置

```c
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
    /* 中断配置 */
    DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_PIN);
    DL_GPIO_enableInterrupt(KEY_PORT, KEY_PIN);
}
```

#### 2.2.3 中断服务函数

```c
// 中断服务函数
void GROUP1_IRQHandler(void)
{
    switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        // 检测KEY是否发生中断
        case KEY_INT_IIDX:
            if(DL_GPIO_readPins(KEY_PORT, KEY_PIN) > 0)
            {
                DL_GPIO_togglePins(LED_PORT, LED_PIN);
            }
        break;    
    }
}
```

#### 2.2.4 主函数测试

```c
#include <ti_msp_dl_config.h>
#include "LED.h"
#include "KEY.h"

int main(void)
{
    SYSCFG_DL_init();    
    LED_Init();
    Key_Init();
    NVIC_EnableIRQ(KEY_INT_IRQN); // Enable KEY interrupt
    while(1);
}
```
