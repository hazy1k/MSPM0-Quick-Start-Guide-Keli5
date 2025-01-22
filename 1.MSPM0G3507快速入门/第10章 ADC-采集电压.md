# 第十章 ADC-采集电压

## 1. 硬件设计

### 1.1 ADC简介

ADC 全称为模拟-数字转换器，是一种用于将模拟信号转换为数字信号的模拟数字转换器。我们知道，模拟信号是连续的，其取值可以在一定范围内任意变化，如声音、光信号等。而数字信号则是离散的二进制信号，如计算机中的数据0和1，仅能取有限的值。

ADC的工作原理是将模拟信号通过采样转换为离散的数字信号，然后再通过量化、编码等处理，最终得到对应的数字表示。ADC采样的频率越高，得到的数字信号就越接近原来的模拟信号，也就是保真度越高，但是需要更多的资源和计算功耗。

ADC 通常用于从外部模拟传感器中读取模拟信号，并将其转换为数字信号供嵌入式系统或计算机进行处理，例如测量温度、湿度、压力等物理量。

### 1.2 MSPM0G系列的ADC介绍

MSPM0G3507采用的是逐次逼近型的12位ADC，它有 17个多路复用通道可以转换。17个外部通道，都对应单片机的某个引脚，这个引脚不是固定的，详情请参考引脚图或者数据手册。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/adc/adc_20240703_160333.png)

各种通道的 A/D 转换可以配置成 **单次、序列转换** 模式。

**单次转换模式：** 每次进行一次ADC转换后，ADC会自动停止，并将结果存储在ADC数据寄存器中。

**重复单次转换模式：** 当ADC完成一次转换后，它会自动启动另一次转换，持续的进行转换，直到外部触发或者软件触发的方式停止连续转换。

**多通道顺序单次转换模式：** 用于对多个输入通道进行依次转换。在该模式下，ADC会根据配置的通道采集顺序，对多个通道进行单次采样并转换。

**多通道顺序重复转换模式：** 用于对多个输入通道进行依次重复转换。在该模式下，ADC会根据配置的通道采集顺序，对多个通道进行重复采样并转换。

### 1.3 ADC的基本参数

**分辨率：** 表示ADC转换器的输出精度，通常以位数（bit）表示，比如8位、10位、12位等，位数越高，精度越高。MSPM0L1306支持8、10、12位的分辨率。

**采样率：** 表示ADC对模拟输入信号进行采样的速率，通常以每秒采样次数（samples per second，SPS）表示，也称为转换速率，表示ADC能够进行多少次模拟到数字的转换。MSPM0G3507的SPS为4Msps。

**电压基准：** ADC的电压基准是用于与模拟输入信号进行比较，从而实现模拟信号到数字信号的转换的一个参考电压。这个基准电压的准确性和稳定性对ADC的转换精度有着决定性的影响。而MSPM0G3507可以支持软件选择三种基准：（1）1.4V 和 2.5V 的可配置内部专用 ADC 基准电压 (VREF)（2）MCU 电源电压 (VDD) （3）通过 VREF+和 VREF- 引脚为 ADC 提供外部基准。如未配置电压基准则默认使用MCU电源电压作为ADC电压基准。

**采样范围：** 指ADC可以采集到的模拟输入信号的电压范围，范围见下：

VREF- ≤ ADC ≤ VREF+

其中VREF- 为设置的电压基准负，通常为0V。VREF+ 为电压基准正，根据软件的配置确定范围。

## 2. 软件设计

### 2.1 编程大纲

1. ti_msp_dl_config文件初始化

2. 重定向printf

3. ADC相关参数宏定义

4. ADC初始化和采样函数

5. 主函数测试

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
    DL_ADC12_reset(ADC0); // 复位ADC0

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB
    DL_UART_Main_enablePower(UART0); // 使能UART0
    DL_ADC12_enablePower(ADC0); // 使能ADC0

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

#### 2.2.2 printf函数重定向

```c
#if !defined(__MICROLIB)
// 如不使用微库，添加如下代码
#if (__ARMCLIB_VERSION <= 6000000)
// 如果编译是AC5，则使用如下代码
// struct __FILE
// {
//    int handle;
// };
#endif

FILE __stdout;  // 声明 __stdout

// 避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
#endif
// printf 重定向函数
int fputc(int ch, FILE *stream)
{
    while (DL_UART_isBusy(UART_INST) == true);  // 等待 UART 可用   
    DL_UART_Main_transmitData(UART_INST, ch);   // 发送数据
    return ch;
}
```

#### 2.2.3 ADC相关参数宏定义

```c
#ifndef __ADC_H
#define __ADC_H

#include "ti_msp_dl_config.h"

/* 基础配置 */
#define ADC_PORT GPIOA
#define ADC_PIN DL_GPIO_PIN_27
#define ADCx ADC0

/* 中断配置 */
#define ADC_IRQN ADC0_INT_IRQn
#define ADCx_IRQHandler ADC0_IRQHandler

/* 通道配置 */
#define ADC_CHx DL_ADC12_MEM_IDX_0 // channel 0内存索引
#define ADC_REF DL_ADC12_REFERENCE_VOLTAGE_VDDA // 参考电压(VDDA)
#define ADC_REF_VOL -1 // VDDA电压为外部提供，无法定义参考电压值，故定义为-1

void ADC_Init(void);
uint32_t ADC_getValue(void);

#endif /* __ADC_H */

```

#### 2.2.4 ADC初始化

```c
static const DL_ADC12_ClockConfig gADC_VOLTAGEClockConfig = {
    .clockSel = DL_ADC12_CLOCK_SYSOSC, // 选用系统振荡器作为时钟源
    .divideRatio = DL_ADC12_CLOCK_DIVIDE_8, // 时钟分频系数为8
    .freqRange = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32, // 时钟频率范围为24-32MHz
};

void ADC_Init(void)
{
    DL_ADC12_setClockConfig(ADCx, (DL_ADC12_ClockConfig*)&gADC_VOLTAGEClockConfig); // ADC时钟配置
    // 单次采样初始化
    /*1.选择ADC，2.选择模式，3.选择触发方式，4.设置分辨率，5，设置数据格式*/
    // ADC0,重复模式,自动选择采样源,软件触发,12位分辨率,无符号数据格式
    DL_ADC12_initSingleSample(ADCx,DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, 
    DL_ADC12_TRIG_SRC_SOFTWARE, DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    // 转换通道配置
    /*1.选择ADC，2.通道选择，3.输入通道选择，4.参考电压，5.采样定时器选择，6.平均模式，7.内部温度传感器选择，8.触发模式，9.窗口比较模式*/
    // ADC0,通道0，输出通道0，参考电压VDDA，采样定时器来源SCOMP0，平均模式关闭，禁用内部温度传感器，自动触发，窗口比较模式关闭
    DL_ADC12_configConversionMem(ADCx, ADC_CHx, DL_ADC12_INPUT_CHAN_0, ADC_REF,
    DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED, DL_ADC12_BURN_OUT_SOURCE_DISABLED, 
    DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    // 设置采样时间
    DL_ADC12_setSampleTime0(ADCx, 40000);
    /* 中断配置 */
    DL_ADC12_clearInterruptStatus(ADCx, (DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED)); // 清除中断标志位
    DL_ADC12_enableInterrupt(ADCx, (DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
    DL_ADC12_enableConversions(ADCx); // 使能ADC转换
}
/*
时钟设置：选择时钟源和分频比。
采样设置：配置 ADC 工作模式，采样源、触发源、数据格式等。
中断设置：使能 ADC 中断，在转换结果可用时触发中断。
转换启动：启动 ADC 的转换过程。 
*/
```

#### 2.2.5 采样函数及中断服务函数

```c
volatile bool gCheckADC; // ADC转换完成标志位
// 读取ADC数据
uint32_t ADC_getValue(void)
{
    uint32_t value = 0;
    DL_ADC12_startConversion(ADCx); // 启动ADC转换
    while(gCheckADC == false){
        __WFE(); // 等待ADC转换完成
    }
    value = DL_ADC12_getMemResult(ADCx, ADC_CHx); // 获取ADC转换结果
    gCheckADC = false; // 清除标志位
    return value;
}

// ADC中断服务程序
void ADCx_IRQHandler(void)
{
    switch(DL_ADC12_getPendingInterrupt(ADCx)) // 判断ADC是否发生中断
    {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED: // 转换结果可用
            gCheckADC = true; // 设置标志位
            break;
        default:
            break;    
    }
}
```

#### 2.2.6 主函数测试

```c
#include "SysTick.h"
#include <ti_msp_dl_config.h>
#include "ADC.h"
#include "Uart.h"

int main(void)
{
    uint16_t adc_value = 0; // ADC采集值
    uint16_t volt_value = 0; // 转换成电压值
    SYSCFG_DL_init();
    ADC_Init();
    UART_0_init();
    SysTick_Init();
    NVIC_ClearPendingIRQ(UART_IRQN);
    NVIC_EnableIRQ(UART_IRQN);
    NVIC_EnableIRQ(ADC_IRQN);
    while(1)
    {
        adc_value = ADC_getValue(); // 采集ADC值
        printf("ADC value: %d\r\n", adc_value);
        delay_ms(1000);
        volt_value = (int)((adc_value/4095.0*3.3)*100); // 转换成电压值
        printf("Voltage value: %d.%d%d V\r\n",
        volt_value/100,volt_value/10%10,volt_value%10);
        delay_ms(1000);
    }
}

```

## 3. 小结

MSPM0G3507 的 ADC 使用过程涉及多个方面的配置，包括时钟启动、输入引脚设置、转换模式选择、采样时间调整等。合理配置 ADC 可确保其精准地将模拟信号转换为数字信号，适用于各种嵌入式应用，如传感器数据采集、信号监测等。
