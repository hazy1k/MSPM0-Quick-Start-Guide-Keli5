#include "LED.h"

void LED_Init(void) // LED初始化
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // 初始化数字输出引脚
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // 清除PA14上的数据
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // 使能PA14输出
}
