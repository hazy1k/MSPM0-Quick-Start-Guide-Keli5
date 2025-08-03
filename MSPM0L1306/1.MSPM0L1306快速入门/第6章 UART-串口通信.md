# 第六章 UART-串口通信

## 1. 硬件设计

在开发板上有集成了一块CH340E，CH340E 是一款流行的 USB 转串口芯片，广泛用于各种需要串口通信的设备中。它提供了一种低成本、便捷的方式将 USB 接口转换成标准的串口（RS232/RS485/TTL），广泛应用于单片机开发、嵌入式系统、通信设备等多个领域。在开发板上，我们可以通过一个数据线，接入到开发板的TYPE-C接口上，即可与开发板进行串口通信。而CH340E接入到了PA22与PA23引脚。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/uart/uart_20240805_120726.png)

PA22与PA23引脚为L1306的串口0引脚，但是PA23引脚同时也是参考电压和BSL串口引脚。这里默认使用为串口引脚，如果需要使用参考电压，请将R24（0Ω）去除。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/uart/uart_20240805_120738.png)

## 2. 软件设计

### 2.1 编程大纲

1. UART_0相关参数宏定义

2. UART_)基础配置

3. 串口中断服务函数

4. printf函数重定义

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 串口相关参数宏定义

```c
/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_22
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_23
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM23)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM24)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM23_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM24_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_4_MHZ_9600_BAUD                                         (26)
#define UART_0_FBRD_4_MHZ_9600_BAUD                                          (3)
```

#### 2.2.2 串口配置

```c
// 时钟配置
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_MFCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

// 工作参数配置
static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};
void SYSCFG_DL_UART_0_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);
    DL_UART_Main_setClockConfig(UART_0_INST, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);
    DL_UART_Main_init(UART_0_INST, (DL_UART_Main_Config *) &gUART_0Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 9600
     *  Actual baud rate: 9598.08
     */
    DL_UART_Main_setOversampling(UART_0_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_0_INST, UART_0_IBRD_4_MHZ_9600_BAUD, UART_0_FBRD_4_MHZ_9600_BAUD);

    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_RX);
    DL_UART_Main_enable(UART_0_INST);
}
```

#### 2.2.3 串口功能函数

```c
// 串口发送单个字节
void uart0_send_char(char ch)
{
    while( DL_UART_isBusy(UART_0_INST) == true );
    DL_UART_Main_transmitData(UART_0_INST, ch);

}

// 串口发送字符串
void uart0_send_string(char* str)
{

    while(*str!=0&&str!=0)
    {
        uart0_send_char(*str++);
    }
}

// 兼容性相关
#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)
struct __FILE
{
    int handle;
};
#endif
FILE __stdout;
void _sys_exit(int x)
{
    x = x;
}
#endif

// 重定向printf
int fputc(int ch, FILE *stream)
{
    while( DL_UART_isBusy(UART_0_INST) == true);
    DL_UART_Main_transmitData(UART_0_INST, ch);
    return ch;
}
```

#### 2.2.4 串口中断服务函数

```c
// 串口中断服务函数
void UART_0_INST_IRQHandler(void)
{
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX:
            uart_data = DL_UART_Main_receiveData(UART_0_INST);
            uart0_send_char(uart_data);
            break;

        default:
            break;
    }
}
```

#### 2.2.5 主函数测试

```c
#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "LED.h"
#include "SYSTICK.h"
#include "UART.h"

int main(void)
{
    SYSCFG_DL_init();
    LED_Init();
    SysTick_init();
    SYSCFG_DL_UART_0_init();
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    while (1) 
    {            
        LED_ON();
        printf("LED ON\r\n");
        delay_ms(1000);
        LED_OFF();
        printf("LED OFF\r\n");
        delay_ms(1000);
    }
}
```
