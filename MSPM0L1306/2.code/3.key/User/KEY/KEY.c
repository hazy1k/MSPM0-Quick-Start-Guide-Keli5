#include "KEY.h"

void Key_Init(void)
{
    // GPIO状态配置
    DL_GPIO_initDigitalInputFeatures(
        KEY_IOMUX, 
        DL_GPIO_INVERSION_DISABLE, 
        DL_GPIO_RESISTOR_PULL_DOWN, 
        DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE
    );
    DL_GPIO_setUpperPinsPolarity(KEY_PORT, DL_GPIO_PIN_18_EDGE_RISE); // 上升沿检测
}
// DL_GPIO_initDigitalInputFeatures：初始化GPIO输入功能
// KEY_IOMUX：引脚索引
// DL_GPIO_INVERSION_DISABLE：不反转
// DL_GPIO_RESISTOR_PULL_DOWN：下拉
// DL_GPIO_HYSTERESIS_DISABLE：不启用滞后
// DL_GPIO_WAKEUP_DISABLE：不启用唤醒功能
// DL_GPIO_setUpperPinsPolarity：设置组内引脚的极性
// KEY_PORT：引脚组
// DL_GPIO_PIN_18_EDGE_RISE：18号引脚上升沿检测