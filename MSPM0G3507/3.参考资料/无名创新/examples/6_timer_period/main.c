#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "stdio.h"



#define DELAY (3200000)
uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//中断使能
	DL_TimerG_startCounter(TIMER_0_INST);//开始计数
	while(1)
	{
		LCD_clear_L(0,0);
		display_6_8_number_f1(0,0,cnt++);//显示计数器值
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMERG_IIDX_ZERO:
				{
					DL_GPIO_togglePins(PORTB_PORT,PORTB_RED_PIN);//IO电平翻转
				}
				break;
        default:
            break;
    }
}


