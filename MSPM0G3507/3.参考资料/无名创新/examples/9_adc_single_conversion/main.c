#include "ti_msp_dl_config.h"
#include "systick.h"
#include "drv_oled.h"
#include "stdio.h"


volatile bool gCheckADC=false;
volatile uint16_t gAdcResult;
#define DELAY (3200000)
systime main_t,timer_t;
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	oled_init();//oled��ʾ����ʼ��
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//�ж�ʹ��
	DL_TimerG_startCounter(TIMER_0_INST);//��ʼ����
	NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
	while(1)
	{
		LCD_clear_L(0,0);	display_6_8_number_f1(0,0,main_t.current_time);
		LCD_clear_L(0,1);	display_6_8_number_f1(0,1,main_t.period);
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,timer_t.period);
		LCD_clear_L(0,3);	display_6_8_number_f1(0,3,gAdcResult);
		delay_ms(200);
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO��ƽ��ת
		get_systime(&main_t);
		
		DL_ADC12_startConversion(ADC12_0_INST);	//��ʼת��
		/* Wait until all data channels have been loaded. */
		while (gCheckADC == false);
		gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, ADC12_0_ADCMEM_0);//ͨ��PA25��ȡADֵ
		DL_ADC12_enableConversions(ADC12_0_INST);
	}
}


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMERG_IIDX_ZERO:
				{
					get_systime(&timer_t);//������ʱ��������
					DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_RED_PIN);//IO��ƽ��ת
				}
				break;
        default:
            break;
    }
}


void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}