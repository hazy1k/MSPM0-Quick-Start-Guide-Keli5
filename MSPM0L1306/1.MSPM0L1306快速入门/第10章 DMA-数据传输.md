# 第十章 DMA-数据传输

## 1. DMA介绍

MSPM0L1306的DMA控制器具有以下特点：

- 7个独立的传输通道；
- 可以配置的DMA通道优先级；
- 支持8位(byte)，16位(short word)、32位(word)和64位(long word)或者混合大小(byte 和 word)传输；
- 支持最大可达64K任意数据类型的数据块传输；
- 可配置的DMA传输触发源；
- 6种灵活的寻址模式；
- 单次或者块传输模式； 它共有7个DMA通道，各个通道可以独立配置，多种多样的数据传输模式可以适应不同应用场景的数据传输需要。

通过查看TI的数据手册，DMA功能除了常见的内存与外设间的地址寻址方式，还提供了Fill Mode和Table Mode两种拓展模式，DMA通道分为基本类型和全功能类型两种。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/dma/dma_20240805_142055.png)

## 2. 软件设计

### 2.1 编程大纲

1. ADC_DMA相关参数宏定义

2. ADC配置

3. DMA配置

4. ADC采集均值滤波

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 相关参数宏定义

```c
/* Defines for ADC_VOLTAGE */
#define ADC_VOLTAGE_INST                                                    ADC0
#define ADC_VOLTAGE_INST_IRQHandler                              ADC0_IRQHandler
#define ADC_VOLTAGE_INST_INT_IRQN                                (ADC0_INT_IRQn)
#define ADC_VOLTAGE_ADCMEM_ADC_CH0                            DL_ADC12_MEM_IDX_0
#define ADC_VOLTAGE_ADCMEM_ADC_CH0_REF           DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_VOLTAGE_ADCMEM_ADC_CH0_REF_VOLTAGE                             -1   // VDDA cannot be determined
#define GPIO_ADC_VOLTAGE_C0_PORT                                           GPIOA
#define GPIO_ADC_VOLTAGE_C0_PIN                                   DL_GPIO_PIN_27

/* DMA */
#define DMA_CHAN_ID (0)
#define ADC_DMAx (DMA_ADC0_EVT_GEN_BD_TRIG) 
```

#### 2.2.2 ADC配置

```c
/* ADC_VOLTAGE Initialization */
static const DL_ADC12_ClockConfig gADC_VOLTAGEClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_8,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
void ADC_VOLTAGE_init(void)
{
    DL_ADC12_setClockConfig(ADC_VOLTAGE_INST, (DL_ADC12_ClockConfig *) &gADC_VOLTAGEClockConfig);
    DL_ADC12_initSingleSample(ADC_VOLTAGE_INST,
        DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_SOFTWARE,
        DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    DL_ADC12_configConversionMem(ADC_VOLTAGE_INST, ADC_VOLTAGE_ADCMEM_ADC_CH0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setSampleTime0(ADC_VOLTAGE_INST,40000);
    DL_ADC12_enableDMA(ADC_VOLTAGE_INST);
    DL_ADC12_setDMASamplesCnt(ADC_VOLTAGE_INST,1);
    DL_ADC12_enableDMATrigger(ADC_VOLTAGE_INST,(DL_ADC12_DMA_MEM0_RESULT_LOADED));
    DL_ADC12_enableConversions(ADC_VOLTAGE_INST);
}
```

#### 2.2.3 DMA配置

```c
/* DMA_CH0 Initialization */
static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode   = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_HALF_WORD,
    .srcWidth       = DL_DMA_WIDTH_HALF_WORD,
    .trigger        = ADC_DMAx,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

void DMA_CH0_init(void)
{
    DL_DMA_setTransferSize(DMA, DMA_CHAN_ID, 10); 
    DL_DMA_initChannel(DMA, DMA_CHAN_ID , (DL_DMA_Config *) &gDMA_CH0Config);
}
```

#### 2.2.4 均值滤波

```c
// 均值滤波
uint16_t adc_getvalue(uint16_t number)
{
    uint16_t adc_result = 0;
    uint16_t i = 0;
    for(i = 0; i < number; i++)
    {
        adc_result += ADC_VALUE[i];
    }
    return adc_result/number;
}
```

#### 2.2.5 主函数测试

```c
int main(void)
{
	uint32_t adc_value = 0;
	uint32_t voltage_value = 0;
	uint16_t i = 0;

	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	DL_UART_0_init();
	ADC_VOLTAGE_init();
	DMA_CH0_init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	// 设置DMA搬运的起始地址(ADC采集数据)
	DL_DMA_setSrcAddr(DMA, DMA_CHAN_ID, (uint32_t)&ADC0->ULLMEM.MEMRES[0]);
	// 设置DMA搬运的目标地址(存储到ADC_VALUE数组)
	DL_DMA_setDestAddr(DMA, DMA_CHAN_ID, (uint32_t)&ADC_VALUE[0]);
	DL_DMA_enableChannel(DMA, DMA_CHAN_ID);
	DL_ADC12_startConversion(ADC0);
	printf("ADC init success!\r\n");
	while(1) 
	{
		adc_value = adc_getvalue(10); // 10次滤波
		printf("adc value: %d\r\n", adc_value);
		voltage_value = (int)((adc_value/4095.0*3.3)*100);
		printf("voltage value: %d.%d V\r\n", voltage_value/100,
				voltage_value%100);
		delay_ms(1000);
	}
}

```
