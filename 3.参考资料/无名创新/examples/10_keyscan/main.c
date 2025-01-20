#include "ti_msp_dl_config.h"


#define DELAY (16000000)//0.5s

void delay_ms(uint16_t ms)
{
	while(ms--)
		delay_cycles(CPUCLK_FREQ/1000);
}
int main(void)
{
	SYSCFG_DL_init(); //оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	while(1)
	{
		if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//����Ϊ�ߵ�ƽ
		{
			delay_ms(10);
			if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//��ʱ֮���ٴμ��
			{
				while(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN));//�ȴ������ͷ�
				DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_R_PIN);//��ƽ��ת����LED	
			}
		}
		
		if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//����Ϊ�͵�ƽ
		{
			delay_ms(10);
			if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//��ʱ֮���ٴμ��
			{
				while(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN));//�ȴ������ͷ�
				DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_G_PIN);//��ƽ��ת����LED	
			}
		}	

		delay_ms(100);	
		DL_GPIO_togglePins(GPIO_LEDS_PORT,GPIO_LEDS_USER_TEST_PIN);//��ƽ��ת����LED			
	}
}
