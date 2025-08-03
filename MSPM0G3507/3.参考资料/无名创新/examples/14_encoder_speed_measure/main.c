#include "ti_msp_dl_config.h"
#include "drv_oled.h"

#define DELAY (3200000)
uint32_t cnt=0;
int32_t _encoder_l_count = 0;
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	oled_init();//oled��ʾ����ʼ��
	NVIC_EnableIRQ(USER_GPIO_INT_IRQN);
	while(1)
	{
		LCD_clear_L(0,0);display_6_8_number_f1(0,0,cnt++);//��ʾ������ֵ
		LCD_clear_L(0,1);display_6_8_number_f1(0,1,_encoder_l_count);//��ʾ�����������С�
		delay_cycles(DELAY);//��ʱ0.1��
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO��ƽ��ת
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
			x=1;//A�жϣ���x=1;B�жϣ���x=0;
			
			//�жϷ���ʱ��A��ߵ�ƽ��y=1����֮y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//�жϷ���ʱ��B��ߵ�ƽ��z=1����֮z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;
			
			sum=x+y+z;//����ж�ת������ż����ת��������ת
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_A_PIN);
		}
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN))
		{
			x=0;//A�жϣ���x=1;B�жϣ���x=0;
			//�жϷ���ʱ��A��ߵ�ƽ��y=1����֮y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//�жϷ���ʱ��B��ߵ�ƽ��z=1����֮z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;

			sum=x+y+z;//����ж�ת������ż����ת��������ת
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN);
		}
				
		break;
	}
}