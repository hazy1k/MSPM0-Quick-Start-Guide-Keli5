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
