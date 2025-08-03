#include "ADC.h"

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
/*
    时钟设置：选择时钟源和分频比。  
    采样设置：配置 ADC 工作模式，采样源、触发源、数据格式等。
    中断设置：使能 ADC 中断，在转换结果可用时触发中断。
    转换启动：启动 ADC 的转换过程。 
*/
