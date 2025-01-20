#include "ti_msp_dl_config.h"
#include "drv_oled.h"


void US_100_Start(void);



#define DELAY (3200000)
uint32_t cnt=0;
uint8_t us100_prase_cnt=2;
uint8_t us100_buf[2]={0};
float us100_distance=0;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
  NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
	while(1)
	{
		LCD_clear_L(0,0);
		display_6_8_number_f1(0,0,cnt++);//显示计数器值
		LCD_clear_L(0,1);
		display_6_8_string(0,1,"us100_distance");
		LCD_clear_L(0,2);
		display_6_8_number_f1(0,2,us100_distance);
		display_6_8_string(50,2,"cm");
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
		if(us100_prase_cnt==2) 
		{
			us100_prase_cnt=0;
			US_100_Start();	
			
		}
	}
}



void UART_1_INST_IRQHandler(void)
{
  switch (DL_UART_Main_getPendingInterrupt(UART_1_INST))
  {
    case DL_UART_MAIN_IIDX_RX:
    {
      uint8_t ch = DL_UART_Main_receiveData(UART_1_INST);
			if(us100_prase_cnt==0) 
			{
				us100_prase_cnt=1;
				us100_buf[0]=ch;
			}
			else if(us100_prase_cnt==1) 
			{
				us100_buf[1]=ch;
				us100_distance=(256*(us100_buf[0])+(us100_buf[1]))/10.0f;//单位cm
				us100_prase_cnt=2;
			}
    }
    break;
    default:
      break;
  }
}

#define US_100_Distance_CMD    0x55
#define US_100_Temperature_CMD 0x50
void US_100_Start(void)
{	
  DL_UART_Main_transmitData(UART_1_INST,US_100_Distance_CMD);
}


