#ifndef __TIMER_H
#define __TIMER_H

#include "ti_msp_dl_config.h"

/* TIM相关参数宏定义 */
#define TIMx (TIMG0) // TIMx 选择
#define TIM_IRQN (TIMG0_INT_IRQn) // TIMx 中断向量号
#define TIM_IRQHandler TIMG0_IRQHandler // TIMx 中断处理函数
#define TIM_LOAD_VAL (39999U) // 定时器计数值

void Tim_Init(void);

#endif /* __TIMER_H */
