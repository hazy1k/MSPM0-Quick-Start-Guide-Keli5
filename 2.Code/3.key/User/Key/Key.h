#ifndef __KEY_H
#define __KEY_H

#include "ti_msp_dl_config.h"

#define KEY_PROT GPIOA
#define KEY_PIN DL_GPIO_PIN_18
#define KEY_IOMUX IOMUX_PINCM40

void KEY_Init(void);

#endif 
