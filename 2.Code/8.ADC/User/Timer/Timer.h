#ifndef __TIMER_H
#define __TIMER_H

#include "ti_msp_dl_config.h"

/* TIM相关参数宏定义 */
#define TIM_PWM TIMG12 // 使用的定时器TIMG12
#define TIM_PWM_IRQN (TIMG12_INT_IRQn) // 使用的定时器中断
#define TIM_PWM_IRQHandler TIMG12_IRQHandler // 使用的定时器中断处理函数
#define TIM_PWM_CLKFREQ 4000000 // 定时器时钟频率4MHz

/* TIM输出通道相关参数宏定义 */
#define PWM_PORT GPIOA // 使用的GPIO端口
#define PWM_PIN DL_GPIO_PIN_14 // 使用的GPIO引脚
#define PWM_IOMUX (IOMUX_PINCM36) // 使用的GPIO IOMUX配置
#define PWM_IOMUX_FUNC IOMUX_PINCM36_PF_TIMG12_CCP0 // 使用的GPIO IOMUX功能配置
#define PWM_IDX DL_TIMER_CC_0_INDEX // 使用的定时器输出通道索引

void Tim_Init(void);

#endif /* __TIMER_H */
