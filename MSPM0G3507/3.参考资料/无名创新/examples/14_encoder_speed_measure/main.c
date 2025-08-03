#include "ti_msp_dl_config.h"
#include "drv_oled.h"

#define DELAY (3200000)
uint32_t cnt=0;
int32_t _encoder_l_count = 0;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	NVIC_EnableIRQ(USER_GPIO_INT_IRQN);
	while(1)
	{
		LCD_clear_L(0,0);display_6_8_number_f1(0,0,cnt++);//显示计数器值
		LCD_clear_L(0,1);display_6_8_number_f1(0,1,_encoder_l_count);//显示脉冲数量，有±
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}

uint8_t x,y,z,sum;
void GROUP1_IRQHandler(void)
{
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
	{
		case USER_GPIO_INT_IIDX:
		if(DL_GPIO_getEnabledInterruptStatus(GPIOA, USER_GPIO_PULSE_A_PIN))
		{
			x=1;//A中断，则x=1;B中断，则x=0;
			
			//中断发生时，A相高电平，y=1；反之y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//中断发生时，B相高电平，z=1；反之z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;
			
			sum=x+y+z;//求和判断转动方向，偶数正转，奇数反转
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_A_PIN);
		}
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN))
		{
			x=0;//A中断，则x=1;B中断，则x=0;
			//中断发生时，A相高电平，y=1；反之y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//中断发生时，B相高电平，z=1；反之z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;

			sum=x+y+z;//求和判断转动方向，偶数正转，奇数反转
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN);
		}
				
		break;
	}
}