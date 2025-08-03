#include "ti_msp_dl_config.h"
#include "systick.h"
#include "drv_oled.h"
#include "stdio.h"
#include <string.h>

uint8_t ppm_update_flag=0;
uint16_t Receiver_PPM_Databuf[10]={0};
static uint16_t PPM_buf[10]={0}; 


#define DELAY (3200000)
systime main_t,timer_t;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);//中断使能
	DL_TimerG_startCounter(TIMER_0_INST);//开始计数
	NVIC_EnableIRQ(USER_GPIO_INT_IRQN);//GPIO中断使能
	while(1)
	{
		LCD_clear_L(0,0);	display_6_8_number_f1(0,0,main_t.current_time);
		LCD_clear_L(0,1);	display_6_8_number_f1(0,1,main_t.period);
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,timer_t.period);
		
		LCD_clear_L(0,3);	display_6_8_number_f1(0,3,PPM_buf[0]);		display_6_8_number_f1(50,3,PPM_buf[1]);	
		LCD_clear_L(0,4);	display_6_8_number_f1(0,4,PPM_buf[2]);    display_6_8_number_f1(50,4,PPM_buf[3]);	
		LCD_clear_L(0,5);	display_6_8_number_f1(0,5,PPM_buf[4]);    display_6_8_number_f1(50,5,PPM_buf[5]);	
		LCD_clear_L(0,6);	display_6_8_number_f1(0,6,PPM_buf[6]);    display_6_8_number_f1(50,6,PPM_buf[7]);	
		
		delay_ms(50);
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
		get_systime(&main_t);
	}
}


void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMERG_IIDX_ZERO:
				{
					get_systime(&timer_t);//测量定时器的周期
					DL_GPIO_togglePins(PORTB_PORT,PORTB_RED_PIN);//IO电平翻转
				}
				break;
        default:
            break;
    }
}


void GROUP1_IRQHandler(void)
{
	switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
	{
		case USER_GPIO_INT_IIDX:
		{
			if(DL_GPIO_getEnabledInterruptStatus(USER_GPIO_PORT, USER_GPIO_PPM_PIN))
			{
				static uint32_t last_ppm_time=0,now_ppm_time=0;
				static uint8_t ppm_ready=0,ppm_sample_cnt=0;;
				static uint16_t ppm_time_delta=0;	

				last_ppm_time=now_ppm_time;//系统运行时间获取，单位us
				now_ppm_time=micros();//单位us	
				ppm_time_delta=now_ppm_time-last_ppm_time;//获取时间间隔
				//PPM解析开始
				if(ppm_ready==1)
				{
					if(ppm_time_delta>=2200)//帧结束电平至少2ms=2000us，由于部分老版本遥控器、
						//接收机输出PPM信号不标准，当出现解析异常时，尝试改小此值，该情况仅出现一例：使用天地飞老版本遥控器
					{
						//memcpy(PPM_Databuf,PPM_buf,ppm_sample_cnt*sizeof(uint16));
						ppm_ready = 1;
						ppm_sample_cnt=0;
						ppm_update_flag=1;
					}
					else if(ppm_time_delta>=900&&ppm_time_delta<=2100)
					{         
						PPM_buf[ppm_sample_cnt++]=ppm_time_delta;//对应通道写入缓冲区       
						if(ppm_sample_cnt>=8)//单次解析结束
						{
							memcpy(Receiver_PPM_Databuf,PPM_buf,ppm_sample_cnt*sizeof(uint16_t));
							//ppm_ready=0;
							ppm_sample_cnt=0;
						}
					}
					else  ppm_ready=0;
				}
				else if(ppm_time_delta>=2200)//帧结束电平至少2ms=2000us
				{
					ppm_ready=1;
					ppm_sample_cnt=0;
					ppm_update_flag=0;
				}
				
				DL_GPIO_clearInterruptStatus(USER_GPIO_PORT, USER_GPIO_PPM_PIN);
			}		
		
		}
		break;
	}
}