#include "ti_msp_dl_config.h"
#include "lcd_st7789.h"
#include "bmp.h"


u8 ref = 0; //ˢ����ʾ
u16 vx = 15542, vy = 11165; //�������ӣ���ֵ����1000֮���ʾ���ٸ�ADֵ����һ�����ص�
u16 chx = 140, chy = 146; //Ĭ�����ص�����Ϊ0ʱ��AD��ʼֵ
void xianshi()//��ʾ��Ϣ
{
  u16 lx, ly;
  BACK_COLOR = WHITE;
  POINT_COLOR = RED;
  showhanzi(10, 0, 3); //��
  showhanzi(45, 0, 4); //��
  LCD_ShowString(10, 35, "1.3 IPS-SPI 240*240");
  LCD_ShowString(10, 55, "LCD_W:");
  LCD_ShowNum(70, 55, LCD_W, 3);
  LCD_ShowString(110, 55, "LCD_H:");
  LCD_ShowNum(160, 55, LCD_H, 3);
  lx = 10;
  ly = 75;
}



void showimage() //��ʾ40*40ͼƬ
{
  int i, j, k;
  for(k = 2; k < 6; k++)
  {
    for(j = 0; j < 6; j++)
    {
      Address_set(40 * j, 40 * k, 40 * j + 39, 40 * k + 39);		//��������

      for(i = 0; i < 1600; i++)
      {
        LCD_WR_DATA8(image[i * 2 + 1]);
        LCD_WR_DATA8(image[i * 2]);
      }
    }
  }
  ref = 0;
}



#define DELAY (3200000)
uint32_t cnt = 0;
int main(void)
{
  SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	DL_TimerA_startCounter(TIMER_1_INST);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	Lcd_Init();			//��ʼ��TFT
	LCD_Clear(WHITE); //����
  while(1)
  {
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		xianshi(); //��ʾ��Ϣ
		showimage(); //��ʾ40*40ͼƬ
  }
}

void TIMER_1_INST_IRQHandler(void)
{
  switch (DL_TimerG_getPendingInterrupt(TIMER_1_INST))
  {
    case DL_TIMERG_IIDX_ZERO:
    {
			DL_GPIO_togglePins(USER_GPIO_PORT, USER_GPIO_LED_PA0_PIN); //IO��ƽ��ת
    }
    break;

    default:
      break;
  }
}