#include "LED.h"
#include "ti_msp_dl_config.h"
#include "Uart.h"
#include "SysTick.h"

int main()
{
    SYSCFG_DL_init();
	LED_Init();
	UART_0_init();
    SysTick_Init();
    NVIC_ClearPendingIRQ(UART_IRQN); // Clear any pending interrupts
    NVIC_EnableIRQ(UART_IRQN); // Enable the interrupt
    while(1)
    {
        DL_GPIO_setPins(LED_PORT, LED_PIN);
        delay_ms(1000);
        DL_GPIO_clearPins(LED_PORT, LED_PIN);
        delay_ms(1000);
    }
}
