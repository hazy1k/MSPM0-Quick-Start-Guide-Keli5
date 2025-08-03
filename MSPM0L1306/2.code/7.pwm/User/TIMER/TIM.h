#ifndef __TIM_H
#define __TIM_H

#include <ti_msp_dl_config.h>

/* 定时器0基础配置 */
#define TIMERx (TIMG0)
#define TIMERx_IRQHandler TIMG0_IRQHandler
#define TIMER_IRQn TIMG0_INT_IRQn
#define TIMER_LOAD_VALUE (39999U)

/* PWM通道(TIMG1)配置 */
#define PWM_TIM TIMG1
#define PWM_TIM_IRQHandler TIMG1_IRQHandler
#define PWM_TIM_IRQn TIMG1_INT_IRQn
#define PWM_TIM_CLK_FREQ 100000
#define PWM_PORT GPIOA
#define PWM_PIN DL_GPIO_PIN_14
#define PWM_IOMUX (IOMUX_PINCM15)
#define PWM_IOMUX_FUNC IOMUX_PINCM15_PF_TIMG1_CCP0
#define PWM_IDX DL_TIMER_CC_0_INDEX

void TIMER_Init(void);
void PWM_LED_init(void) ;

#endif
