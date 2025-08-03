#include "ti_msp_dl_config.h"
#include "LED.h"
#include "SYSTICK.h"
#include "UART.h"
#include "ADC.h"

volatile bool gCheckADC; // ADC采集数据标志

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
