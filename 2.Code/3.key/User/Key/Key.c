#include "Key.h"

void KEY_Init(void)
{
    uint32_t pincmIndex = KEY_IOMUX; // 目标引脚
    DL_GPIO_INVERSION inversion = DL_GPIO_INVERSION_DISABLE; // 设置为不反转输入
    DL_GPIO_RESISTOR internalResistor = DL_GPIO_RESISTOR_PULL_DOWN; // 设置为下拉电阻
    DL_GPIO_HYSTERESIS hysteresis = DL_GPIO_HYSTERESIS_DISABLE; // 禁用滞后
    DL_GPIO_WAKEUP wakeup = DL_GPIO_WAKEUP_DISABLE; // 禁用唤醒功能
    DL_GPIO_initDigitalInputFeatures(pincmIndex, inversion, internalResistor, hysteresis, wakeup);
}
