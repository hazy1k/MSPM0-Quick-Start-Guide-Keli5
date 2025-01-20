#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "stdio.h"



#define DELAY (3200000)
uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	oled_init();//oled��ʾ����ʼ��
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//�ж�ʹ��
	DL_TimerG_startCounter(TIMER_0_INST);//��ʼ����
	while(1)
	{
		LCD_clear_L(0,0);
		display_6_8_number_f1(0,0,cnt++);//��ʾ������ֵ
		delay_cycles(DELAY);//��ʱ0.1��
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO��ƽ��ת
	}
}


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMERG_IIDX_ZERO:
				{
					DL_GPIO_togglePins(PORTB_PORT,PORTB_RED_PIN);//IO��ƽ��ת
				}
				break;
        default:
            break;
    }
}


