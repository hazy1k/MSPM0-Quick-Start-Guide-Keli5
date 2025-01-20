#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "bsp_eeprom.h"


float eeprom_write[6]={1.1f,2.2f,3.3f,4.4f,5.5f,6.6f};
float eeprom_read[6]={0};

#define DELAY (3200000)
uint32_t cnt=0;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	bsp_eeprom_init(); //EEPROM初始化
	
	//数据写入
	WriteFlashParameter(0,eeprom_write[0]);
	WriteFlashParameter_Two(1,eeprom_write[1],eeprom_write[2]);
	WriteFlashParameter_Three(3,eeprom_write[3],eeprom_write[4],eeprom_write[5]);
  //数据读取
	ReadFlashParameterThree(0,&eeprom_read[0],&eeprom_read[1],&eeprom_read[2]);
	ReadFlashParameterTwo(3,&eeprom_read[3],&eeprom_read[4]);
	ReadFlashParameterOne(5,&eeprom_read[5]);

	while(1)
	{
		LCD_clear_L(0,0);	display_6_8_number_f1(0,0,cnt++);//显示计数器值
		LCD_clear_L(0,1); display_6_8_string(0,1,"write");	display_6_8_string(60,1,"read");
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,eeprom_write[0]);  display_6_8_number_f1(60,2,eeprom_read[0]);
		LCD_clear_L(0,3);	display_6_8_number_f1(0,3,eeprom_write[1]);  display_6_8_number_f1(60,3,eeprom_read[1]);
		LCD_clear_L(0,4);	display_6_8_number_f1(0,4,eeprom_write[2]);  display_6_8_number_f1(60,4,eeprom_read[2]);
		LCD_clear_L(0,5);	display_6_8_number_f1(0,5,eeprom_write[3]);  display_6_8_number_f1(60,5,eeprom_read[3]);
		LCD_clear_L(0,6);	display_6_8_number_f1(0,6,eeprom_write[4]);  display_6_8_number_f1(60,6,eeprom_read[4]);
		LCD_clear_L(0,7);	display_6_8_number_f1(0,7,eeprom_write[5]);  display_6_8_number_f1(60,7,eeprom_read[5]);
		
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}

