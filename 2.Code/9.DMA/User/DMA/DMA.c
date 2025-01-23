#include "DMA.h"

// DMA工作参数配置(传输方式、传输模式、数据宽度、地址递增、触发方式等)
static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode  = DL_DMA_FULL_CH_REPEAT_BLOCK_TRANSFER_MODE, // 重复传输
    .extendedMode  = DL_DMA_NORMAL_MODE, // 基础模式
    .destIncrement = DL_DMA_ADDR_INCREMENT, // 目标地址递增
    .srcIncrement  = DL_DMA_ADDR_UNCHANGED, // 源地址不变
    .destWidth     = DL_DMA_WIDTH_HALF_WORD, // 目标数据宽度为半字
    .srcWidth      = DL_DMA_WIDTH_HALF_WORD, // 源数据宽度为半字
    .trigger       = ADC_DMA_EVENT, // ADC采样触发DMA
    .triggerType   = DL_DMA_TRIGGER_TYPE_EXTERNAL, // 外部触发
};

void DMA_CH0_Init(void)
{
    DL_DMA_setTransferSize(DMA, DMA_CH0_ID, 10); // 设置DMA传输大小为10个数据块
    DL_DMA_initChannel(DMA, DMA_CH0_ID, (DL_DMA_Config*)&gDMA_CH0Config);
}
