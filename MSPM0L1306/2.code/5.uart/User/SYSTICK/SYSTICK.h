#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <ti_msp_dl_config.h>

void SysTick_init(void);
void delay_ms(uint32_t ms);
void SysTick_Handler(void);

#endif 
