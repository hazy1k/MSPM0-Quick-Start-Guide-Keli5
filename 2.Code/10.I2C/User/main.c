#include <ti_msp_dl_config.h>
#include "SysTick.h"
#include "Uart.h"
#include "SHT20.h"

#define T_ADDR 0xf3 // SHT20温度地址
#define PH_ADDR 0xf5 // SHT20湿度地址

int main(void)
{
    SYSCFG_DL_init();
    UART_0_init();
    I2C_GPIO_Init();
    SysTick_Init();
    NVIC_ClearPendingIRQ(UART_IRQN);
    NVIC_EnableIRQ(UART_IRQN);
    while (1)
    {
        uint32_t TEMP = SHT20_Read(T_ADDR) * 100;
        uint32_t PH = SHT20_Read(PH_ADDR) * 100;
        printf("Temperature: %d.%d C\n", TEMP / 100, TEMP % 100);
        printf("Humidity: %d.%d %%\n", PH / 100, PH % 100);
        delay_ms(1000);
   }
}
