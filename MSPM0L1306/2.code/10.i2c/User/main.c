#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "LED.h"
#include "SYSTICK.h"
#include "UART.h"
#include "SHT20.h"

#define T_ADDR 0xf3 // 温度
#define PH_ADDR 0xf5 // 湿度

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	SYSCFG_DL_UART_0_init();
	I2C_GPIO_Init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	printf("SHT20 Start\r\n");
	while (1) 
	{			
        uint32_t TEMP = SHT20_Read(T_ADDR) * 100;
        uint32_t PH   = SHT20_Read(PH_ADDR) * 100;
        printf("温度 = %d.%02d °c\r\n", TEMP/100,TEMP%100);
        printf("湿度 = %d.%02d %%RH\r\n", PH/100,PH%100);
        printf("\n");
        delay_ms(1000);
	}
}
