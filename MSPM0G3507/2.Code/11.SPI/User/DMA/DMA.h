#ifndef __DMA_H
#define __DMA_H

#include "ti_msp_dl_config.h"

// DMA通道标识符
#define DMA_CH0_ID (0)
// ADC转换触发DMA事件
#define ADC_DMA_EVENT (DMA_ADC0_EVT_GEN_BD_TRIG)

void DMA_CH0_Init(void);

#endif 
