#include <ti_msp_dl_config.h>
#include "SysTick.h"
#include "ADC.h"
#include "Uart.h"
#include "DMA.h"
#include "LED.h"

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
