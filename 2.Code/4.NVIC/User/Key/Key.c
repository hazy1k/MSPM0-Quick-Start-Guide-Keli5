#include "Key.h"

void KEY_Init(void)
{
    uint32_t pincmIndex = KEY_IOMUX; // 目标引脚
    DL_GPIO_INVERSION inversion = DL_GPIO_INVERSION_DISABLE; // 设置为不反转输入
    DL_GPIO_RESISTOR internalResistor = DL_GPIO_RESISTOR_PULL_DOWN; // 设置为下拉电阻
    DL_GPIO_HYSTERESIS hysteresis = DL_GPIO_HYSTERESIS_DISABLE; // 禁用滞后
    DL_GPIO_WAKEUP wakeup = DL_GPIO_WAKEUP_DISABLE; // 禁用唤醒功能
    DL_GPIO_initDigitalInputFeatures(pincmIndex, inversion, internalResistor, hysteresis, wakeup); // 初始化IO功能

    /* 中断相关配置 */
    DL_GPIO_setUpperPinsPolarity(KEY_PORT, DL_GPIO_PIN_18_EDGE_RISE_FALL); // 设置中断引脚的上升沿和下降沿触发中断
    DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_PIN); // 清除中断标志位
    NVIC_SetPriority(KEY_IRQN, 2); // 设置中断优先级
    DL_GPIO_enableInterrupt(KEY_PORT, KEY_PIN); // 使能中断
}
