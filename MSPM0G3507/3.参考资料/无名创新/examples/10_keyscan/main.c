#include "ti_msp_dl_config.h"


#define DELAY (16000000)//0.5s

void delay_ms(uint16_t ms)
{
	while(ms--)
		delay_cycles(CPUCLK_FREQ/1000);
}
int main(void)
{
	SYSCFG_DL_init(); //芯片资源初始化,由SysConfig配置软件自动生成
	while(1)
	{
		if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//按下为高电平
		{
			delay_ms(10);
			if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//延时之后再次检测
			{
				while(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN));//等待按键释放
				DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_R_PIN);//电平翻转驱动LED	
			}
		}
		
		if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//按下为低电平
		{
			delay_ms(10);
			if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//延时之后再次检测
			{
				while(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN));//等待按键释放
				DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_G_PIN);//电平翻转驱动LED	
			}
		}	

		delay_ms(100);	
		DL_GPIO_togglePins(GPIO_LEDS_PORT,GPIO_LEDS_USER_TEST_PIN);//电平翻转驱动LED			
	}
}
