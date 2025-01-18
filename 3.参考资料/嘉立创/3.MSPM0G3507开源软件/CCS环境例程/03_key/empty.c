#include "ti_msp_dl_config.h"

int main(void)
{

    SYSCFG_DL_init();

    while (1)
    {
        //如果读取到的引脚值大于0，说明PA18引脚为高电平
        if( DL_GPIO_readPins(KEY_PORT, KEY_PIN_18_PIN) > 0 )
        {
            DL_GPIO_setPins(LED_PORT,LED_PIN_14_PIN);  //LED控制输出高电平
        }
        else//如果PA18引脚为低电平
        {
            DL_GPIO_clearPins(LED_PORT,LED_PIN_14_PIN);//LED控制输出低电平
        }
    }
}