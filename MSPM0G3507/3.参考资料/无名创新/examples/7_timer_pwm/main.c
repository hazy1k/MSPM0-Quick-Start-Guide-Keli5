#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "stdio.h"

uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	oled_init();//oled��ʾ����ʼ��
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//�ж�ʹ��
	DL_TimerG_startCounter(TIMER_0_INST);//��ʼ����
	DL_TimerG_startCounter(PWM_1_INST);
	while(1)
	{
		LCD_clear_L(0,0);
		display_6_8_number_f1(0,0,cnt++);//��ʾ������ֵ
		if(cnt<100)
		{			
			DL_TimerG_setCaptureCompareValue(PWM_1_INST, 10*cnt, GPIO_PWM_1_C0_IDX);//TIMA1-CH0-PB26
			DL_TimerG_setCaptureCompareValue(PWM_1_INST, 10*cnt, GPIO_PWM_1_C1_IDX);//TIMA1-CH1-PB27
		}
		else if(cnt<110)
		{
			DL_TimerG_setCaptureCompareValue(PWM_1_INST, 0, GPIO_PWM_1_C0_IDX);//TIMA1-CH0-PB26
			DL_TimerG_setCaptureCompareValue(PWM_1_INST, 0, GPIO_PWM_1_C1_IDX);//TIMA1-CH1-PB27
		}
		else cnt=0;
	}
}


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMERG_IIDX_ZERO:
				{
					DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO��ƽ��ת
				}
				break;
        default:
            break;
    }
}


