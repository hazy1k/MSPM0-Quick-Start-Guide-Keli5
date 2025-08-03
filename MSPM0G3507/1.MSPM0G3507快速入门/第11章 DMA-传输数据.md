# 第十一章 DMA-传输数据

## 1. 硬件设计

### 1.1 DMA简介

DMA（Direct Memory Access）控制器提供了一种硬件的方式在外设和存储器之间或者存储器和存储器之间传输数据，而无需CPU的介入，避免了CPU多次进入中断进行大规模的数据拷贝，最终提高整体的系统性能。

DMA是一种能够在无需CPU参与的情况下，将数据块在内存和外设之间高效传输的硬件机制。实现这种功能的集成电路单元叫做DMA Controller，即DMA控制器。

DMA控制器在没有CPU参与的情况下从一个地址向另一个地址传输数据，它支持多种数据宽度，突发类型，地址生成算法，优先级和传输模式，可以灵活的配置以满足应用的需求。

### 1.2 MSPM0G3507的DMA介绍

MSPM0G3507的DMA控制器具有以下特点：

- 7个独立的传输通道；
- 可以配置的DMA通道优先级；
- 支持8位(byte)，16位(short word)、32位(word)和64位(long word)或者混合大小(byte 和 word)传输；
- 支持最大可达64K任意数据类型的数据块传输；
- 可配置的DMA传输触发源；
- 6种灵活的寻址模式；
- 单次或者块传输模式； 它共有7个DMA通道，各个通道可以独立配置，多种多样的数据传输模式可以适应不同应用场景的数据传输需要。

通过查看TI的数据手册，DMA功能除了常见的内存与外设间的地址寻址方式，还提供了Fill Mode和Table Mode两种拓展模式，DMA通道分为基本类型和全功能类型两种。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/dma/dma_20240708_121533.png)

通过查看数据手册，只有全功能类型的DMA通道支持重复传输、提前中断以及拓展模式，基本功能的DMA通道支持基本的数据传输和中断，但是足够满足简单的数据传输要求。

### 1.3 DMA传输方式

MSPM0G3507的DMA支持四种传输模式，每个通道可单独配置其传输模式。例如，通道 0 可配置为单字或单字节传输模式，而通道 1 配置为块传输模式，通道 2 采用重复块传输模式。 传输模式独立于寻址模式进行配置。 任何寻址模式都可以与任何传输模式一同使用。可以传输三种类型的数据，可以通过 .srcWidth 和 .destWidth 控制位进行选择。源位置和目标位置可以是字节、短字或字数据。也支持以字节到字节、短字到短字、字到字或任何组合的方式进行传输。如下：

- 单字或单字节传输：每次传输都需要一个单独的触发。当 DMAxSZ 传输已经生成时 DMA 会被自动关闭。
- 块传输：一个整块将会在一个触发后传输。在块传输结束时 DMA 会被自动关闭。
- 重复单字或单字节传输：每次传输都需要一个单独的触发，DMA保持启用状态。
- 重复块传输：一个整块将会在一个触发后传输，DMA保持启用状态。

## 2. 软件设计

### 2.1 编程大纲

1. ADC配置DMA

2. DMA工作参数配置，初始化

3. ADC均值滤波函数

4. 主函数测试

### 2.2 代码分析

#### 2.2.1 ADC配置DMA

```c
static const DL_ADC12_ClockConfig gADC_VOLTAGEClockConfig = {
    .clockSel    = DL_ADC12_CLOCK_SYSOSC, // 选用系统振荡器作为时钟源
    .divideRatio = DL_ADC12_CLOCK_DIVIDE_8, // 时钟分频系数为8
    .freqRange   = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32, // 时钟频率范围为24-32MHz
};

void ADC_Init(void)
{
    DL_ADC12_setClockConfig(ADCx, (DL_ADC12_ClockConfig*)&gADC_VOLTAGEClockConfig); // ADC时钟配置
    // 单次采样初始化
    /*1.选择ADC，2.选择模式，3.选择触发方式，4.设置分辨率，5，设置数据格式*/
    // ADC0,重复模式,自动选择采样源,软件触发,12位分辨率,无符号数据格式
    DL_ADC12_initSingleSample(ADCx, DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, 
    DL_ADC12_TRIG_SRC_SOFTWARE, DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    // 转换通道配置
    /*1.选择ADC，2.通道选择，3.输入通道选择，4.参考电压，5.采样定时器选择，6.平均模式，7.内部温度传感器选择，8.触发模式，9.窗口比较模式*/
    // ADC0,通道0，输出通道0，参考电压VDDA，采样定时器来源SCOMP0，平均模式关闭，禁用内部温度传感器，自动触发，窗口比较模式关闭
    DL_ADC12_configConversionMem(ADCx, ADC_CHx, DL_ADC12_INPUT_CHAN_0, ADC_REF,
    DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED, DL_ADC12_BURN_OUT_SOURCE_DISABLED, 
    DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    // 设置采样时间
    DL_ADC12_setSampleTime0(ADCx, 40000);
    // 使能DMA传输
    DL_ADC12_enableDMA(ADCx);
    DL_ADC12_setDMASamplesCnt(ADCx, 1); // 每次传输一个数据
    DL_ADC12_enableDMATrigger(ADCx, (DL_ADC12_DMA_MEM0_RESULT_LOADED)); // 设置DMA触发源为转换结果可用
    /*
        启用 DMA 以便 ADC 完成转换时自动传输数据。
        设置 DMA 每次传输一个 ADC 转换结果。
        配置 DMA 触发条件，当 ADC MEM0 存储转换结果后触发 DMA 传输。
        启动 ADC 转换，开始采样并自动通过 DMA 传输数据。
    */
    DL_ADC12_enableConversions(ADCx); // 使能ADC转换
}
```

#### 2.2.2 DMA工作参数配置及初始化

```c
#include "DMA.h"

// DMA工作参数配置(传输方式、传输模式、数据宽度、地址递增、触发方式等)
static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode  = DL_DMA_FULL_CH_REPEAT_BLOCK_TRANSFER_MODE, // 重复传输
    .extendedMode  = DL_DMA_NORMAL_MODE, // 基础模式
    .destIncrement = DL_DMA_ADDR_INCREMENT, // 目标地址递增
    .srcIncrement  = DL_DMA_ADDR_UNCHANGED, // 源地址不变
    .destWidth     = DL_DMA_WIDTH_HALF_WORD, // 目标数据宽度为半字
    .srcWidth      = DL_DMA_WIDTH_HALF_WORD, // 源数据宽度为半字
    .trigger       = ADC_DMA_EVENT, // ADC采样触发DMA
    .triggerType   = DL_DMA_TRIGGER_TYPE_EXTERNAL, // 外部触发
};

void DMA_CH0_Init(void)
{
    DL_DMA_setTransferSize(DMA, DMA_CH0_ID, 10); // 设置DMA传输大小为10个数据块
    DL_DMA_initChannel(DMA, DMA_CH0_ID, (DL_DMA_Config*)&gDMA_CH0Config);
}
```

#### 2.2.3 均值滤波采集ADC值

```c
// 均值滤波求ADC数据
uint32_t ADC_filter(uint32_t number)
{
    uint32_t sum = 0;
    uint32_t result = 0;
    uint32_t i;
    for(i = 0; i < number; i++)
    {
        sum += ADC_VALUE[i];
    }
    result = sum/number;
    return result;
}
```

#### 2.2.4 主函数测试

```c
volatile uint16_t ADC_VALUE[20]; // DMA搬运的目的地址
uint32_t ADC_filter(uint32_t number);

int main(void)
{
    uint16_t adc_value = 0; // ADC采集值
    uint16_t volt_value = 0; // 转换成电压值
    SYSCFG_DL_init();
    LED_Init();
    ADC_Init();
    UART_0_init();
    SysTick_Init();
    DMA_CH0_Init();
    NVIC_ClearPendingIRQ(UART_IRQN);
    NVIC_EnableIRQ(UART_IRQN);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_ID, (uint32_t)&ADC0->ULLMEM.MEMRES[0]); // 设置DMA搬运的起始的地址
    /*
        ADC0 是 ADC 控制器的实例。
        ULLMEM.MEMRES 是 ADC 模块的一个结果寄存器，它保存 ADC 转换的输出数据。
        MEMRES[0] 是 ADC 转换结果的第一个位置，通常是转换的第一个通道的结果。   
        将这个地址转换成 uint32_t 类型，然后传递给 DMA，表示 DMA 需要从这个地址读取数据。
    */
    DL_DMA_setDestAddr(DMA, DMA_CH0_ID, (uint32_t)&ADC_VALUE[0]); // 设置DMA搬运的目的地址
    DL_DMA_enableChannel(DMA, DMA_CH0_ID); // 使能DMA通道0
    DL_ADC12_startConversion(ADCx); // 启动ADC转换
    while(1)
    {
        adc_value = ADC_filter(10); // 10次平均滤波
        printf("ADC value: %d\r\n", adc_value);
        delay_ms(1000);
        volt_value = (int)((adc_value/4095.0*3.3)*100); // 转换成电压值
        printf("Voltage value: %d.%d%d V\r\n",
        volt_value/100,volt_value/10%10,volt_value%10);
        delay_ms(1000);
        DL_GPIO_togglePins(LED_PORT, LED_PIN);
        delay_ms(1000);
    }
}
```

## 3. 小结

### 3.1 **启用 DMA 控制器和通道**

首先需要启用 DMA 控制器以及选择适当的 DMA 通道。MSPM0G3507 支持多个 DMA 通道，可以根据需求选择合适的通道。

### 3.2 **配置 DMA 通道的源地址和目标地址**

- **源地址 (Src)**：DMA 从外设或内存读取数据的地址。
- **目标地址 (Dest)**：DMA 将数据写入的内存地址。

### 3.3 **配置 DMA 传输的大小**

你需要配置每次 DMA 传输的数据大小，比如是一个字节、一个字、一个长字（32位）等。

### 3.4 **设置 DMA 传输的方向**

DMA 支持不同的数据传输方向，例如从外设到内存（外设到内存传输）或从内存到外设（内存到外设传输）。

### 3.5 **设置触发条件**

DMA 通常需要某个事件触发，如 ADC 转换完成、定时器溢出等。你需要配置 DMA 触发源。

### 3.6 **启用 DMA 和外设**

启动 DMA 和相关外设，以开始数据传输。

### 3.7 **处理中断 (如果需要)**

如果 DMA 配置了中断，可以在中断服务例程中处理数据传输完成后的操作
