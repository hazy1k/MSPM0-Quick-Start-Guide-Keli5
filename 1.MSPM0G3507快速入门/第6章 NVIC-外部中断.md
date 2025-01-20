# 第六章 NVIC-外部中断

## 1. 硬件设计

外部中断是硬件中断的一种，它由微控制器外部的事件引发。微控制器的某些引脚被设计为对特定事件的发生做出响应，例如按钮的按压、传感器的信号改变等。这些指定的引脚通常被称为“外部中断引脚”。

MSPM0系列把CPU中断，DMA触发，以及外设触发外设，都统称为事件（Event），触发源称为事件发布者（Event Publisher），相应中断的外设被称为事件订阅者（Event Subscriber）。

下图为MSPM0系列的事件安排表。其中绿色表示事件发布者（发送），将外设上发生的事件告诉总线；红色为事件订阅者（接收），接收总线上的事件情况。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/ccs-beginner/exti/exti_20240820_175552.png)

本章与key硬件设计相同，只是在软件上有区别，上一章我们使用的读取GPIO状态，这次我们使用外部中断配置

我们配置好了引脚的中断之后，还要手动编写外部中断的中断服务函数，因为我们开启的引脚下降沿中断，当我们的按键从按下到松开的时候产生的下降沿，就会触发一次中断，触发中断就会执行中断服务函数。各个中断的中断服务函数名称已经被写死，不可修改，否则无法正常进入中断服务函数。关于中断服务函数的名称是什么,看下表：

| 中断函数名             | 中断说明              |
| ----------------- | ----------------- |
| Reset_Handler     | 复位中断函数            |
| NMI_Handler       | 不可屏蔽中断函数          |
| HardFault_Handler | 硬件故障中断函数          |
| SVC_Handler       | 特权中断函数            |
| PendSV_Handler    | 一种可挂起的、最低优先级的中断函数 |
| SysTick_Handler   | 滴答定时器中断函数         |
| GROUP0_IRQHandler | GROUP0的中断函数       |
| GROUP1_IRQHandler | GROUP1中断函数        |
| TIMG8_IRQHandler  | TIMG8的中断函数        |
| UART3_IRQHandler  | UART3的中断函数        |
| ADC0_IRQHandler   | ADC0的中断函数         |
| ADC1_IRQHandler   | ADC1的中断函数         |
| CANFD0_IRQHandler | CANFD0的中断函数       |
| DAC0_IRQHandler   | DAC0的中断函数         |
| SPI0_IRQHandler   | SPI0的中断函数         |
| SPI1_IRQHandler   | SPI1的中断函数         |
| UART1_IRQHandler  | UART1的中断函数        |
| UART2_IRQHandler  | UART2的中断函数        |
| UART0_IRQHandler  | UART0的中断函数        |
| TIMG0_IRQHandler  | TIMG0 的中断函数       |
| TIMG6_IRQHandler  | TIMG6的中断函数        |
| TIMA0_IRQHandler  | TIMA0的中断函数        |
| TIMA1_IRQHandler  | TIMA1的中断函数        |
| TIMG7_IRQHandler  | TIMG7的中断函数        |
| TIMG12_IRQHandler | TIMG12的中断函数       |
| I2C0_IRQHandler   | I2C0的中断函数         |
| I2C1_IRQHandler   | I2C1的中断函数         |
| AES_IRQHandler    | 硬件加速器的中断函数        |
| RTC_IRQHandler    | RTC实时时钟的中断函数      |
| DMA_IRQHandler    | DMA的中断函数          |

## 2. 软件设计

### 2.1 编程大纲

1. 中断配置宏定义

2. 中断配置

3. 中断服务函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 中断配置宏定义

```c
#ifndef __KEY_H
#define __KEY_H

#include "ti_msp_dl_config.h"

#define KEY_PORT GPIOA
#define KEY_PIN DL_GPIO_PIN_18
#define KEY_IOMUX IOMUX_PINCM40

/* 中断相关配置 */
#define KEY_IRQN (GPIOA_INT_IRQn) // 选择中断源
#define KEY_INT_IIDX (DL_INTERRUPT_GROUP1_IIDX_GPIOA) // 选择中断组
#define KEY_IIDX (DL_GPIO_IIDX_DIO18) // 选择中断引脚

void KEY_Init(void);

#endif 

```

#### 2.2.2 中断配置

```c
#include "Key.h"

void KEY_Init(void)
{
    uint32_t pincmIndex = KEY_IOMUX; // 目标引脚
    DL_GPIO_INVERSION inversion = DL_GPIO_INVERSION_DISABLE; // 设置为不反转输入
    DL_GPIO_RESISTOR internalResistor = DL_GPIO_RESISTOR_PULL_DOWN; // 设置为下拉电阻
    DL_GPIO_HYSTERESIS hysteresis = DL_GPIO_HYSTERESIS_DISABLE; // 禁用滞后
    DL_GPIO_WAKEUP wakeup = DL_GPIO_WAKEUP_DISABLE; // 禁用唤醒功能
    DL_GPIO_initDigitalInputFeatures(pincmIndex, inversion, internalResistor, hysteresis, wakeup); // 初始化IO功能

    /* 中断相关配置 */
    DL_GPIO_setUpperPinsPolarity(KEY_PORT, DL_GPIO_PIN_18_EDGE_RISE_FALL); // 设置中断引脚的上升沿和下降沿触发中断
    DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_PIN); // 清除中断标志位
    NVIC_SetPriority(KEY_IRQN, 2); // 设置中断优先级
    DL_GPIO_enableInterrupt(KEY_PORT, KEY_PIN); // 使能中断
}

```

#### 2.2.3 中断服务函数

```c
// Group1的中断服务函数
void GROUP1_IRQHandler(void)
{ 
	// 读取KEY_IRQN中断的状态
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
	{
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
int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	KEY_Init();
	NVIC_EnableIRQ(KEY_IRQN); // 使能KEY_IRQN中断
	while(1)
	{
	}
}
```

## 3. 小结

### 3.1 **中断处理流程**

- **中断使能**：每个中断源都可以通过设置相应的使能位来启用或禁用。
- **中断触发**：每个中断源都具有触发条件（如边沿触发或电平触发）。
- **中断响应**：当中断触发时，处理器会中断当前任务，保存当前上下文并跳转到中断服务程序（ISR）。
- **中断嵌套**：MSPM0G3507 支持嵌套中断，这意味着高优先级的中断可以打断低优先级的中断。

### 3.2 **中断配置**

- **NVIC 配置**：通过 NVIC 寄存器配置中断源、优先级和使能。
- **外设中断配置**：外部设备（如 GPIO、定时器、串口等）通过外设控制寄存器配置其中断使能、触发模式等。
- **中断向量表**：每个中断源在中断向量表中都有一个对应的地址，程序通过该地址跳转到相应的中断服务例程。

```c
#include "msp.h"

void GPIOA_Handler(void) {
    if (GPIOA->RIS & 0x01) {  // 检查 PA0 中断标志
        GPIOA->ICR |= 0x01;  // 清除中断标志
        // 执行相关操作，如切换 LED 状态等
    }
}

void TIMER0A_Handler(void) {
    if (TIMER0->RIS & 0x01) {  // 检查定时器中断标志
        TIMER0->ICR |= 0x01;  // 清除中断标志
        // 执行定时任务，如切换 LED 状态等
    }
}

int main(void) {
    // 配置 GPIOA 引脚为输入并使能中断
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R0;
    GPIOA->DIR &= ~0x01;
    GPIOA->IS &= ~0x01;
    GPIOA->IBE &= ~0x01;
    GPIOA->IEV |= 0x01;
    GPIOA->IM |= 0x01;
    NVIC_EnableIRQ(GPIOA_IRQn);
    NVIC_SetPriority(GPIOA_IRQn, 2);

    // 配置定时器0
    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;
    TIMER0->CTL = 0;
    TIMER0->CFG = 0x04;
    TIMER0->TAMR = 0x02;
    TIMER0->TAILR = 50000000 - 1;
    TIMER0->IMR = 0x01;
    NVIC_EnableIRQ(TIMER0A_IRQn);
    NVIC_SetPriority(TIMER0A_IRQn, 1);

    // 启动定时器
    TIMER0->CTL |= 0x01;

    while (1) {
        // 主循环，等待中断
    }
}

```
