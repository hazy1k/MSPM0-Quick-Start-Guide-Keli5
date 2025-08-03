#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "stdio.h"

void usart0_send_bytes(unsigned char *buf, int len);
int fputc(int ch, FILE *f);
uint8_t send_test[13]={"hello world!\n"};

#define DELAY (3200000)
uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//оƬ��Դ��ʼ��,��SysConfig��������Զ�����
	oled_init();//oled��ʾ����ʼ��
	while(1)
	{
		LCD_clear_L(0,0);
		display_6_8_number_f1(0,0,cnt++);//��ʾ������ֵ
		delay_cycles(DELAY);//��ʱ0.1��
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO��ƽ��ת
		usart0_send_bytes(send_test,sizeof(send_test));
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

