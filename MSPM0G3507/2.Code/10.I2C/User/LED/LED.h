#ifndef __LED_H
#define __LED_H

#include "ti_msp_dl_config.h"

#define LED_PORT GPIOA
#define LED_PIN DL_GPIO_PIN_14
#define LED_IOMUX IOMUX_PINCM36

void LED_Init(void);

#endif 
