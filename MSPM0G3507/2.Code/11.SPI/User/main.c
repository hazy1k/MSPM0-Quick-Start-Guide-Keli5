#include <ti_msp_dl_config.h>
#include "SysTick.h"
#include "Uart.h"
#include "W25Q32.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    SYSCFG_DL_init();
    UART_0_init();
    SysTick_Init();
    SPIx_Init();
    uint8_t buff[10] = {0};        
    W25Q32_read(buff, 0, 5);
    printf("buff = %s\r\n",buff);				
    W25Q32_write((uint8_t*)"6666", 0, 5);
    delay_ms(100);
    W25Q32_read(buff, 0, 5);
    printf("buff = %s\r\n",buff);
    while (1) 
    {        
    }
}
