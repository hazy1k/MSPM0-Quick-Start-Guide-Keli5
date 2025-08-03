# 第九章 ADC-电压采集

## 1. MSPM0L系列的ADC介绍

MSPM0L1306采用的是逐次逼近型的12位ADC，它有 10 个多路复用通道可以转换。10个外部通道，都对应单片机的某个引脚，这个引脚不是固定的，详情请参考引脚图或者数据手册。

![](https://wiki.lckfb.com/storage/images/zh-hans/dzx-mspm0l1306/beginner/adc/adc_20240805_141152.png)

各种通道的 A/D 转换可以配置成 **单次、序列转换** 模式。

**单次转换模式：** 每次进行一次ADC转换后，ADC会自动停止，并将结果存储在ADC数据寄存器中。

**重复单次转换模式：** 当ADC完成一次转换后，它会自动启动另一次转换，持续的进行转换，直到外部触发或者软件触发的方式停止连续转换。

**多通道顺序单次转换模式：** 用于对多个输入通道进行依次转换。在该模式下，ADC会根据配置的通道采集顺序，对多个通道进行单次采样并转换。

**多通道顺序重复转换模式：** 用于对多个输入通道进行依次重复转换。在该模式下，ADC会根据配置的通道采集顺序，对多个通道进行重复采样并转换。

## 2. 软件设计

### 2.1 编程大纲

1. ADC相关参数宏定义

2. ADC基础配置

3. ADC中断服务函数

4. ADC采集函数

5. 主函数测试

### 2.2 代码分析

#### 2.2.1 ADC相关参数宏定义

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
```

#### 2.2.2 ADC基础配置

```c
/* ADC_VOLTAGE Initialization */
static const DL_ADC12_ClockConfig gADC_VOLTAGEClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC, // 使用系统时钟
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_8, // 8分频
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32, 
};
void ADC_VOLTAGE_init(void)
{
    DL_ADC12_setClockConfig(ADC_VOLTAGE_INST, (DL_ADC12_ClockConfig *) &gADC_VOLTAGEClockConfig); // 时钟配置
    DL_ADC12_initSingleSample(ADC_VOLTAGE_INST,
        DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_SOFTWARE,
        DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED); // 单次采样配置
    DL_ADC12_configConversionMem(ADC_VOLTAGE_INST, ADC_VOLTAGE_ADCMEM_ADC_CH0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED); // 配置ADC通道
    DL_ADC12_setSampleTime0(ADC_VOLTAGE_INST,40000); // 设置采样时间
    /* Enable ADC12 interrupt */
    DL_ADC12_clearInterruptStatus(ADC_VOLTAGE_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
    DL_ADC12_enableInterrupt(ADC_VOLTAGE_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
    DL_ADC12_enableConversions(ADC_VOLTAGE_INST);
}
```

#### 2.2.3 ADC中断服务函数

```c
// ADC中断服务函数
void ADC_VOLTAGE_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_VOLTAGE_INST)) 
    {

        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}
```

#### 2.2.4 ADC采集函数

```c
// 读取ADC采集的数据
uint32_t adc_getValue(void)
{
    uint32_t adc_result = 0;
    DL_ADC12_startConversion(ADC_VOLTAGE_INST);
    while(gCheckADC == false)
    {
        __WFE();
    }
    adc_result = DL_ADC12_getMemResult(ADC_VOLTAGE_INST, ADC_VOLTAGE_ADCMEM_ADC_CH0); // PA27
    gCheckADC = false; // 清除标志位
    return adc_result;
}
```

#### 2.2.5 主函数测试

```c
int main(void)
{
	uint32_t adc_value = 0;
	uint32_t voltage_value = 0;

	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	DL_UART_0_init();
	ADC_VOLTAGE_init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(ADC_VOLTAGE_INST_INT_IRQN);
	while(1) 
	{
		adc_value = adc_getValue();
		printf("adc value: %d\r\n", adc_value);
		voltage_value = (int)((adc_value/4095.0*3.3)*100);
		printf("voltage value: %d.%d V\r\n", voltage_value/100,
				voltage_value%100);
		delay_ms(1000);
	}
}
```
