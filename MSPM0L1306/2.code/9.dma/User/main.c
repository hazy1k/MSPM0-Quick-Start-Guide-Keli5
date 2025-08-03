#include "ti_msp_dl_config.h"
#include "LED.h"
#include "SYSTICK.h"
#include "UART.h"
#include "ADC.h"

volatile bool gCheckADC; // ADC采集数据标志
volatile uint16_t ADC_VALUE[10];

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
