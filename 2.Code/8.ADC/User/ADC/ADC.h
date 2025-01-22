#ifndef __ADC_H
#define __ADC_H

#include "ti_msp_dl_config.h"

/* 基础配置 */
#define ADC_PORT GPIOA
#define ADC_PIN DL_GPIO_PIN_27
#define ADCx ADC0

/* 中断配置 */
#define ADC_IRQN ADC0_INT_IRQn
#define ADCx_IRQHandler ADC0_IRQHandler

/* 通道配置 */
#define ADC_CHx DL_ADC12_MEM_IDX_0 // channel 0内存索引
#define ADC_REF DL_ADC12_REFERENCE_VOLTAGE_VDDA // 参考电压(VDDA)
#define ADC_REF_VOL -1 // VDDA电压为外部提供，无法定义参考电压值，故定义为-1

void ADC_Init(void);
uint32_t ADC_getValue(void);

#endif /* __ADC_H */
