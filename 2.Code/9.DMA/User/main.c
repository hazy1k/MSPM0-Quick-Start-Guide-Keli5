#include <ti_msp_dl_config.h>
#include "SysTick.h"
#include "ADC.h"
#include "Uart.h"
#include "DMA.h"
#include "LED.h"

volatile uint16_t ADC_VALUE[20]; // DMA���˵�Ŀ�ĵ�ַ
uint32_t ADC_filter(uint32_t number);

int main(void)
{
    uint16_t adc_value = 0; // ADC�ɼ�ֵ
    uint16_t volt_value = 0; // ת���ɵ�ѹֵ
    SYSCFG_DL_init();
    LED_Init();
    ADC_Init();
    UART_0_init();
    SysTick_Init();
    DMA_CH0_Init();
    NVIC_ClearPendingIRQ(UART_IRQN);
    NVIC_EnableIRQ(UART_IRQN);
    DL_DMA_setSrcAddr(DMA, DMA_CH0_ID, (uint32_t)&ADC0->ULLMEM.MEMRES[0]); // ����DMA���˵���ʼ�ĵ�ַ
    /*
        ADC0 �� ADC ��������ʵ����
        ULLMEM.MEMRES �� ADC ģ���һ������Ĵ����������� ADC ת����������ݡ�
        MEMRES[0] �� ADC ת������ĵ�һ��λ�ã�ͨ����ת���ĵ�һ��ͨ���Ľ����   
        �������ַת���� uint32_t ���ͣ�Ȼ�󴫵ݸ� DMA����ʾ DMA ��Ҫ�������ַ��ȡ���ݡ�
    */
    DL_DMA_setDestAddr(DMA, DMA_CH0_ID, (uint32_t)&ADC_VALUE[0]); // ����DMA���˵�Ŀ�ĵ�ַ
    DL_DMA_enableChannel(DMA, DMA_CH0_ID); // ʹ��DMAͨ��0
    DL_ADC12_startConversion(ADCx); // ����ADCת��
    while(1)
    {
        adc_value = ADC_filter(10); // 10��ƽ���˲�
        printf("ADC value: %d\r\n", adc_value);
        delay_ms(1000);
        volt_value = (int)((adc_value/4095.0*3.3)*100); // ת���ɵ�ѹֵ
        printf("Voltage value: %d.%d%d V\r\n",
        volt_value/100,volt_value/10%10,volt_value%10);
        delay_ms(1000);
        DL_GPIO_togglePins(LED_PORT, LED_PIN);
        delay_ms(1000);
    }
}

// ��ֵ�˲���ADC����
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
