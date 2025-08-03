#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "stdio.h"

void usart0_send_bytes(unsigned char *buf, int len);
uint8_t send_test[13]={"hello world!\n"};

#define DELAY (3200000)
uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	usart0_send_bytes(send_test,sizeof(send_test));
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);//清中断挂起
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);//使能串口中断
	while(1)
	{
		LCD_clear_L(0,0);//清第一行
		display_6_8_number_f1(0,0,cnt++);//显示计数器值
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}



void usart0_send_bytes(unsigned char *buf, int len)
{
  while(len--)
  {
		DL_UART_Main_transmitDataBlocking(UART_0_INST, *buf);
    buf++;
  }
}


void UART_0_INST_IRQHandler(void)
{
	switch (DL_UART_Main_getPendingInterrupt(UART0)) 
	{
		case DL_UART_MAIN_IIDX_RX:
		{
			uint8_t ch=DL_UART_Main_receiveData(UART_0_INST);//串口数据接受单个字节数据
			DL_UART_Main_transmitData(UART_0_INST, ch);//将本次解析的到数据ch发送出去
		}
		break;
		default:break;
	}
}

