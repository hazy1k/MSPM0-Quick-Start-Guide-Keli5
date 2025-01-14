#include "LED.h"

// LED初始化
void LED_Init(void)
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // 初始化数字输出引脚
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // 清除PA0上的数据
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // 使能PA0输出
}
