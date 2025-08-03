#ifndef __KEY_H
#define __KEY_H

#include <ti_msp_dl_config.h>

/* GPIO相关宏定义*/
#define KEY_PORT (GPIOA)
#define KEY_PIN (DL_GPIO_PIN_18)
#define KEY_IOMUX (IOMUX_PINCM19)

/* 中断相关宏定义*/
#define KEY_INT_IRQN (GPIOA_INT_IRQn)
#define KEY_INT_IIDX (DL_INTERRUPT_GROUP1_IIDX_GPIOA)

void Key_Init(void);

#endif 
