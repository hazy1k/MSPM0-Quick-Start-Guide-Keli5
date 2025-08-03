#include "LED.h"

void LED_Init(void)
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // 使能数字引脚输出功能
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // 置低电平
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // 使能输出
}
