#include "ti_msp_dl_config.h"
#include "lcd_st7789.h"
#include "bmp.h"


u8 ref = 0; //刷新显示
u16 vx = 15542, vy = 11165; //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
u16 chx = 140, chy = 146; //默认像素点坐标为0时的AD起始值
void xianshi()//显示信息
{
  u16 lx, ly;
  BACK_COLOR = WHITE;
  POINT_COLOR = RED;
  showhanzi(10, 0, 3); //电
  showhanzi(45, 0, 4); //子
  LCD_ShowString(10, 35, "1.3 IPS-SPI 240*240");
  LCD_ShowString(10, 55, "LCD_W:");
  LCD_ShowNum(70, 55, LCD_W, 3);
  LCD_ShowString(110, 55, "LCD_H:");
  LCD_ShowNum(160, 55, LCD_H, 3);
  lx = 10;
  ly = 75;
}



void showimage() //显示40*40图片
{
  int i, j, k;
  for(k = 2; k < 6; k++)
  {
    for(j = 0; j < 6; j++)
    {
      Address_set(40 * j, 40 * k, 40 * j + 39, 40 * k + 39);		//坐标设置

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
  SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	DL_TimerA_startCounter(TIMER_1_INST);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	Lcd_Init();			//初始化TFT
	LCD_Clear(WHITE); //清屏
  while(1)
  {
		BACK_COLOR = BLACK;
		POINT_COLOR = WHITE;
		xianshi(); //显示信息
		showimage(); //显示40*40图片
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