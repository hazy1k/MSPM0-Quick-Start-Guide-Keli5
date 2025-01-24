#ifndef __KEY_H
#define __KEY_H

#include "ti_msp_dl_config.h"

#define KEY_PORT GPIOA
#define KEY_PIN DL_GPIO_PIN_18
#define KEY_IOMUX IOMUX_PINCM40

/* 中断相关配置 */
#define KEY_IRQN (GPIOA_INT_IRQn) // 选择中断源
#define KEY_INT_IIDX (DL_INTERRUPT_GROUP1_IIDX_GPIOA) // 选择中断组
#define KEY_IIDX (DL_GPIO_IIDX_DIO18) // 选择中断引脚

void KEY_Init(void);

#endif 
