#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "drv_imu.h"
#include "math.h"


#define DELAY (3200000)
uint32_t cnt=0;
int16_t accel[3],gyro[3];
float temperature;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
  mpu6050_init();//mpu6050初始化
	while(1)
	{
		mpu6050_read(gyro,accel,&temperature);
		//通过三轴加速度计,计算水平观测角度
		float ax,ay,az;
		ax=accel[0];
		ay=accel[1];
		az=accel[2];
		float roll=-57.3f*atan(ax/sqrtf(ay*ay+az*az)); //横滚角
		float pitch= 57.3f*atan(ay/sqrtf(ax*ax+az*az));//俯仰角		
		
		LCD_clear_L(0,0);	display_6_8_number_f1(0,0,cnt++);
		LCD_clear_L(0,1); display_6_8_string(0,1,"accel");	    display_6_8_string(40,1,"gyro");      display_6_8_string(80,1,"temp");
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,accel[0]);  display_6_8_number_f1(40,2,gyro[0]);  display_6_8_number_f1(80,2,temperature);
		LCD_clear_L(0,3);	display_6_8_number_f1(0,3,accel[1]);  display_6_8_number_f1(40,3,gyro[1]);
		LCD_clear_L(0,4);	display_6_8_number_f1(0,4,accel[2]);  display_6_8_number_f1(40,4,gyro[2]);
		
		LCD_clear_L(0,5); display_6_8_string(0,5,"pitch");	    display_6_8_string(40,5,"roll");
		LCD_clear_L(0,6);	display_6_8_number_f1(0,6,pitch);     display_6_8_number_f1(40,6,roll);

		
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}

