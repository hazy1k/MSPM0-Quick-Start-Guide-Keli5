#include "ti_msp_dl_config.h"
#include "drv_oled.h"
#include "drv_imu.h"
#include "math.h"
#include "SPL06.h"

float baro_temp_raw,baro_pressure_raw;

#define DELAY (3200000)
uint32_t cnt=0;
int16_t accel[3],gyro[3];
float temperature;
int main(void)
{
	SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	oled_init();//oled显示屏初始化
	SPL06_Init();//初始化spl06
	while(1)
	{
		SPL06_Read_Data(&baro_temp_raw,&baro_pressure_raw);
    LCD_clear_L(0,0);	display_6_8_number_f1(0,0,cnt++);		
		LCD_clear_L(0,1); display_6_8_string(0,1,"temp");	              display_6_8_string(40,1,"pressure");
		LCD_clear_L(0,2);	display_6_8_number_f1(0,2,baro_temp_raw);     display_6_8_number_f1(40,2,baro_pressure_raw);	
		delay_cycles(DELAY);//延时0.1秒
		DL_GPIO_togglePins(USER_GPIO_PORT,USER_GPIO_LED_PA0_PIN);//IO电平翻转
	}
}

