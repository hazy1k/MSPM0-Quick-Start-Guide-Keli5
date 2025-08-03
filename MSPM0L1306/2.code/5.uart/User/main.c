#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "LED.h"
#include "SYSTICK.h"
#include "UART.h"

int main(void)
{
	SYSCFG_DL_init();
	LED_Init();
	SysTick_init();
	SYSCFG_DL_UART_0_init();
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
	while (1) 
	{			
		LED_ON();
		printf("LED ON\r\n");
		delay_ms(1000);
		LED_OFF();
		printf("LED OFF\r\n");
		delay_ms(1000);
	}
}
