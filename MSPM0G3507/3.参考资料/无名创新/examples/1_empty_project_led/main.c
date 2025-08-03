#include "ti_msp_dl_config.h"
#include "led.h"

#define DELAY (16000000)

int main(void)
{
	SYSCFG_DL_init(); //оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	while(1)
	{
		delay_cycles(DELAY);//��ʱ0.5��
		DL_GPIO_togglePins(PORTA_PORT,PORTA_LED_PIN);//��ƽ��ת����LED		
		DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_R_PIN | PORTB_RGB_G_PIN | PORTB_RGB_B_PIN);//��ƽ��ת����RGB
	}
}

