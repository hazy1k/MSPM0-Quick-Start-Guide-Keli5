#include "ti_msp_dl_config.h"
#include "systick.h"
#include "lcd.h"
#include "lcd_init.h"
#include "pic.h"


uint32_t cnt = 0;
int main(void)
{
	uint8_t i,j;
	float t=0;
  SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成	
	DL_TimerA_startCounter(TIMER_1_INST);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	LCD_Init();
  while(1)
  {
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowChinese(0,0,"中景园电子",RED,WHITE,32,0);
		LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
		LCD_ShowIntNum(48,40,LCD_W,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowString(0,70,"Increaseing Nun:",RED,WHITE,16,0);
		LCD_ShowFloatNum1(128,70,t,4,RED,WHITE,16);
		t+=0.11;
		for(j=0;j<7;j++)
		{
			for(i=0;i<8;i++)
			{
				LCD_ShowPicture(40*i,200+j*40,40,40,gImage_1);
			}
		}
  }
}

void TIMER_1_INST_IRQHandler(void)
{
  switch (DL_TimerG_getPendingInterrupt(TIMER_1_INST))
  {
    case DL_TIMERG_IIDX_ZERO:
    {
			DL_GPIO_togglePins(USER_GPIO_PORT, USER_GPIO_LED_PA0_PIN); //IO电平翻转
    }
    break;

    default:
      break;
  }
}