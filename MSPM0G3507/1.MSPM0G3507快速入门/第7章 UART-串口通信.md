# 第七章 UART-串口通信

## 1. 硬件设计

### 1.1 串口简介

开发板使用的是MSPM0G3507为主控，它只有四个串口可用，分别是UART0到UART3。并且其串口支持的功能非常丰富，根据数据手册中的说明支持的功能有如下表所示：

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/uart/uart_20240626_193139.png)

需要注意的是，不是所有的引脚都支持串口功能，只有特定的引脚才支持。这个需要大家去根据数据手册进行查找。也可以参考下方我们总结的外设引脚分布图。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/uart/uart_20240626_193331.png)

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/uart/uart_20240626_193345.png)

### 1.2 开发板原理图

在开发板上有集成了一块CH340E，CH340E 是一款流行的 USB 转串口芯片，广泛用于各种需要串口通信的设备中。它提供了一种低成本、便捷的方式将 USB 接口转换成标准的串口（RS232/RS485/TTL），广泛应用于单片机开发、嵌入式系统、通信设备等多个领域。**在开发板上，我们可以通过一个数据线，接入到开发板的TYPE-C接口上，即可与开发板进行串口通信。** 而CH340E接入到了PA10与PA11引脚。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/uart/uart_20240626_194711.png)

PA10与PA11引脚为G3507的串口0引脚。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/uart/uart_20240626_194724.png)

## 2. 软件设计

### 2.1 编程大纲

1. ti_msp_dl_config文件配置

2. UART相关参数宏定义

3. UART初始化

4. 编写UART发送数据函数

5. 编写中断服务函数，实现数据接发

### 2.2 代码分析

#### 2.2.1 ti_msp_dl_config.c

```c
#include "ti_msp_dl_config.h"

void SYSCFG_DL_init(void) // 系统初始化
{
    SYSCFG_DL_initPower(); // 电源初始化
    SYSCFG_DL_SYSCTL_init(); // 系统时钟初始化
}

void SYSCFG_DL_initPower(void) // 电源初始化
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_reset(GPIOB); // 复位GPIOB
    DL_UART_Main_reset(UART0); // 复位UART0

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB
    DL_UART_Main_enablePower(UART0); // 使能UART0

    delay_cycles(POWER_STARTUP_DELAY); // 等待启动时间
}

void SYSCFG_DL_SYSCTL_init(void) // 系统时钟初始化
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统频率为32MHz
	DL_SYSCTL_disableHFXT(); // 关闭HFXT
	DL_SYSCTL_disableSYSPLL(); // 关闭系统PLL
    DL_SYSCTL_enableMFCLK(); // 使能主时钟-MFCLK
}
/*
UART有三个时钟源：
1.BUSCLK:内部高速振荡器提供，出厂设置为32MHz
2.LFCLK:内部低速振荡器提供，低功耗时有效（32KHz）
3.MFCLK:固定的4MHz时钟
*/
```

#### 2.2.2 UART相关参数宏定义

```c
/* 中断配置 */
#define UART_INST            UART0
#define UART_IRQN   UART0_INT_IRQn
#define UARTx_IRQHandler UART0_IRQHandler

/* GPIO配置 */
#define UART_RX_PORT  GPIOA
#define UART_TX_PORT  GPIOA
#define UART_RX_PIN   DL_GPIO_PIN_11
#define UART_TX_PIN   DL_GPIO_PIN_10
#define UART_IOMUX_RX (IOMUX_PINCM22)
#define UART_IOMUX_TX (IOMUX_PINCM21)
#define UART_RX_FUNC  IOMUX_PINCM22_PF_UART0_RX
#define UART_TX_FUNC  IOMUX_PINCM21_PF_UART0_TX

/* 串口配置 */
#define UART_BAUD_RATE (9600)
#define UART_IBRD  (26)
#define UART_FBRD  (3)
```

#### 2.2.3 UART工作参数设置

```c
// UART时钟源配置
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_MFCLK, // 时钟源选择为MFCLK
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1 // 时钟分频系数为1
};

// UART工作参数配置
static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL, // 工作模式为正常模式
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX, // 传输方向为TX和RX
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE, // 流控模式为无流控
    .parity      = DL_UART_MAIN_PARITY_NONE, // 校验位为无校验
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS, // 数据位长度为8位
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE // 停止位为1位
};
```

#### 2.2.4 UART初始化

```c
// UART初始化
void UART_0_init(void)
{
    DL_GPIO_initPeripheralOutputFunction(UART_IOMUX_TX, UART_TX_FUNC); // 设置GPIO为输出(TX)
    DL_GPIO_initPeripheralInputFunction(UART_IOMUX_RX, UART_RX_FUNC); // 设置GPIO为输入(RX)
    DL_UART_Main_setClockConfig(UART_INST, (DL_UART_Main_ClockConfig*)&gUART_0ClockConfig); // 时钟源配置
    DL_UART_Main_init(UART_INST, (DL_UART_Main_Config*)&gUART_0Config); // UART工作参数配置
    DL_UART_Main_setOversampling(UART_INST, DL_UART_OVERSAMPLING_RATE_16X); // 采样率配置为16倍
    DL_UART_Main_setBaudRateDivisor(UART_INST, UART_IBRD, UART_FBRD); // 设置波特率分频值
    DL_UART_Main_enableInterrupt(UART_INST, DL_UART_MAIN_INTERRUPT_RX); // 使能接收中断
    DL_UART_Main_enable(UART_INST); // 使能UART
}

```

#### 2.2.5 发送数据函数及中断处理

```c
void uart0_send_char(char ch) // 发送一个字符
{
    while( DL_UART_isBusy(UART_INST) == true );
    DL_UART_Main_transmitData(UART_INST, ch);
}

void uart0_send_string(char* str) // 发送字符串
{
    while(*str!=0&&str!=0)
    {
        uart0_send_char(*str++);
    }
}

// UART中断服务程序
void UARTx_IRQHandler(void)
{
    switch(DL_UART_getPendingInterrupt(UART_INST))
    {
        case DL_UART_IIDX_RX:
            uart_data = DL_UART_Main_receiveData(UART_INST);
            uart0_send_char(uart_data);
            break;
        default:
            break;
    }
}
```

#### 2.2.6 主函数测试

```c
int main()
{
    SYSCFG_DL_init();
	LED_Init();
	UART_0_init();
    SysTick_Init();
    NVIC_ClearPendingIRQ(UART_IRQN); // Clear any pending interrupts
    NVIC_EnableIRQ(UART_IRQN); // Enable the interrupt
    while(1)
    {
        DL_GPIO_setPins(LED_PORT, LED_PIN);
        delay_ms(1000);
        DL_GPIO_clearPins(LED_PORT, LED_PIN);
        delay_ms(1000);
    }
}
```

## 3. 小结

本章主要就是一个配置的事情，配置时钟、初始化 UART 外设、配置波特率、数据位、停止位、校验位等。

- 确保你的系统时钟配置正确，以保证波特率精度。
- 根据实际需求，配置正确的波特率、数据位、停止位和校验位。
- 如果你使用 DMA 或中断方式来接收数据，确保相应的中断和 DMA 通道已正确配置。


