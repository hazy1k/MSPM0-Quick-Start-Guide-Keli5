#ifndef __TIM_H
#define __TIM_H

#include <ti_msp_dl_config.h>

#define TIMERx (TIMG0)
#define TIMERx_IRQHandler TIMG0_IRQHandler
#define TIMER_IRQn TIMG0_INT_IRQn
#define TIMER_LOAD_VALUE (39999U)

void TIMER_Init(void);

#endif
