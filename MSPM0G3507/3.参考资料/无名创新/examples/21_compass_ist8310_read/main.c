#include "ti_msp_dl_config.h"
#include "systick.h"
#include "drv_oled.h"
#include "drv_imu.h"
#include "math.h"
#include "drv_i2c.h"
#include "ist8310.h"

uint8_t Compass_Read_Data(int16_t *mag);


#define DELAY (3200000)
uint32_t cnt=0;
int16_t mag[3];
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	uint8_t val[2]={0xC0,0x01};
	I2C_WriteReg(IST8310_SLAVE_ADDRESS,0x42,&val[0],1);//Set/Reset内部平均
	I2C_WriteReg(IST8310_SLAVE_ADDRESS,IST8310_REG_CNTRL1,&val[1],1);//Single Measurement Mode; 
	while(1)
	{
		Compass_Read_Data(mag);
		float yaw=57.3f*atan2(mag[1],mag[2]);
    LCD_clear_L(0,0);	display_6_8_number_f1(0,0,cnt++);		
		LCD_clear_L(0,1); display_6_8_string(0,1,"magx");	       display_6_8_string(40,1,"magy");    display_6_8_string(80,1,"magz");
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,mag[0]);     display_6_8_number_f1(40,2,mag[1]); display_6_8_number_f1(80,2,mag[2]);	
		
		LCD_clear_L(0,3); display_6_8_string(0,3,"yaw"); 
		LCD_clear_L(0,4); display_6_8_number_f1(0,4,yaw); 
		
		delay_ms(5);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}



uint8_t Compass_Read_Data(int16_t *mag)//读取磁力计数据状态机
{
  static uint16_t compass_sampling_cnt=0;
  uint8_t buf[6];
  compass_sampling_cnt++;
  if(compass_sampling_cnt==1)
  {
		uint8_t val=0x01;
		I2C_WriteReg(IST8310_SLAVE_ADDRESS,IST8310_REG_CNTRL1,&val,1);//Single Measurement Mode
  }
  else if(compass_sampling_cnt>=20)//100ms
  {
		I2C_ReadReg(IST8310_SLAVE_ADDRESS,0x03,buf,6);
    mag[0]= (int16_t)((buf[1]<<8)|buf[0]);
    mag[1]= (int16_t)((buf[3]<<8)|buf[2]);
    mag[2]= (int16_t)((buf[5]<<8)|buf[4]);
	
    compass_sampling_cnt=0;
    return 1;
  }
  return 0;
}


