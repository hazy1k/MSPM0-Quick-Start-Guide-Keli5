#include "ti_msp_dl_config.h"
#include "drv_i2c.h"
#include "SPL06.h"

float  spl_cal_data[9]={0};
#define PRESSURE_REG 0X00
#define TEMP_REG 0X03
#define PRS_CFG 0x06
#define TMP_CFG 0x07
#define MEAS_CFG 0x08
#define SPL06_REST_VALUE 0x09
#define SPL06_REST_REG 0x0C
#define PRODUCT_ID 0X0D
//#define HW_ADR 0x77 //SDO HIGH OR NC
#define HW_ADR (0x76) //SDO LOW
#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1


struct spl0601_calib_param_t {
  int16_t c0;
  int16_t c1;
  int32_t  c00;
  int32_t  c10;
  int16_t c01;
  int16_t c11;
  int16_t c20;
  int16_t c21;
  int16_t c30;
};

struct spl0601_t {
  struct spl0601_calib_param_t calib_param;/**<calibration data*/
  uint8_t chip_id; /**<chip id*/
  int32_t  i32rawPressure;
  int32_t  i32rawTemperature;
  int32_t  i32kP;
  int32_t  i32kT;
};


struct spl0601_t spl0601;
struct spl0601_t *p_spl0601;
void spl0601_write(uint8_t hwadr, uint8_t regadr, uint8_t val);
uint8_t spl0601_read(uint8_t hwadr, uint8_t regadr);
void spl0601_get_calib_param(void);


/*****************************************************************************
函 数 名  : spl0601_write
功能描述  : I2C 寄存器写入子函数
输入参数  : uint8_t hwadr   硬件地址
uint8_t regadr  寄存器地址
uint8_t val     值
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_write(unsigned char hwadr, unsigned char regadr, unsigned char val)
{
	I2C_WriteReg(hwadr,regadr,&val,1);
}



/*****************************************************************************
函 数 名  : spl0601_read
功能描述  : I2C 寄存器读取子函数
输入参数  : uint8_t hwadr   硬件地址
uint8_t regadr  寄存器地址
输出参数  :
返 回 值  : uint8_t 读出值
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
uint8_t spl0601_read(unsigned char hwadr, unsigned char regadr)
{
  uint8_t reg_data;
  I2C_ReadReg(hwadr,regadr,&reg_data,1);
  return reg_data;
}

/*****************************************************************************
函 数 名  : spl0601_init
功能描述  : SPL06-01 初始化函数
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_init(void)
{
  p_spl0601 = &spl0601; /* read Chip Id */
  p_spl0601->i32rawPressure = 0;
  p_spl0601->i32rawTemperature = 0;
  p_spl0601->chip_id = 0x34;
  spl0601_get_calib_param();
}


/*****************************************************************************
函 数 名  : spl0601_rateset
功能描述  :  设置温度传感器的每秒采样次数以及过采样率
输入参数  : uint8_t u8OverSmpl  过采样率         Maximal = 128
uint8_t u8SmplRate  每秒采样次数(Hz) Maximal = 128
uint8_t iSensor     0: Pressure; 1: Temperature
输出参数  : 无
返 回 值  : 无
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月24日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_rateset(uint8_t iSensor, uint8_t u8SmplRate, uint8_t u8OverSmpl)
{
  uint8_t reg = 0;
  int32_t  i32kPkT = 0;
  switch(u8SmplRate)
  {
  case 2:
    reg |= (1<<4);//左移动4位（原5位），原厂提供例程有问题，20180410无名小哥改
    break;
  case 4:
    reg |= (2<<4);
    break;
  case 8:
    reg |= (3<<4);
    break;
  case 16:
    reg |= (4<<4);
    break;
  case 32:
    reg |= (5<<4);
    break;
  case 64:
    reg |= (6<<4);
    break;
  case 128:
    reg |= (7<<4);
    break;
  case 1:
  default:
    break;
  }
  switch(u8OverSmpl)
  {
  case 2:
    reg |= 1;
    i32kPkT = 1572864;
    break;
  case 4:
    reg |= 2;
    i32kPkT = 3670016;
    break;
  case 8:
    reg |= 3;
    i32kPkT = 7864320;
    break;
  case 16:
    i32kPkT = 253952;
    reg |= 4;
    break;
  case 32:
    i32kPkT = 516096;
    reg |= 5;
    break;
  case 64:
    i32kPkT = 1040384;
    reg |= 6;
    break;
  case 128:
    i32kPkT = 2088960;
    reg |= 7;
    break;
  case 1:
  default:
    i32kPkT = 524288;
    break;
  }
  
  if(iSensor == 0)
  {
    p_spl0601->i32kP = i32kPkT;
    spl0601_write(HW_ADR, 0x06, reg);
    if(u8OverSmpl > 8)
    {
      reg = spl0601_read(HW_ADR, 0x09);
      spl0601_write(HW_ADR, 0x09, reg | 0x04);
    }
  }
  if(iSensor == 1)
  {
    p_spl0601->i32kT = i32kPkT;
    spl0601_write(HW_ADR, 0x07, reg|0x80);  //Using mems temperature
    if(u8OverSmpl > 8)
    {
      reg = spl0601_read(HW_ADR, 0x09);
      spl0601_write(HW_ADR, 0x09, reg | 0x08);
    }
  }
}

/*****************************************************************************
函 数 名  : spl0601_get_calib_param
功能描述  : 获取校准参数
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_calib_param(void)
{
  uint32_t  h;
  uint32_t  m;
  uint32_t  l;
  h =  spl0601_read(HW_ADR, 0x10);
  l  =  spl0601_read(HW_ADR, 0x11);
  p_spl0601->calib_param.c0 = (int16_t)h<<4 | l>>4;
  p_spl0601->calib_param.c0 = (p_spl0601->calib_param.c0&0x0800)?(0xF000|p_spl0601->calib_param.c0):p_spl0601->calib_param.c0;
  h =  spl0601_read(HW_ADR, 0x11);
  l  =  spl0601_read(HW_ADR, 0x12);
  p_spl0601->calib_param.c1 = (int16_t)(h&0x0F)<<8 | l;
  p_spl0601->calib_param.c1 = (p_spl0601->calib_param.c1&0x0800)?(0xF000|p_spl0601->calib_param.c1):p_spl0601->calib_param.c1;
  h =  spl0601_read(HW_ADR, 0x13);
  m =  spl0601_read(HW_ADR, 0x14);
  l =  spl0601_read(HW_ADR, 0x15);
  p_spl0601->calib_param.c00 = (int32_t)h<<12 | (int32_t)m<<4 | (int32_t)l>>4;
  p_spl0601->calib_param.c00 = (p_spl0601->calib_param.c00&0x080000)?(0xFFF00000|p_spl0601->calib_param.c00):p_spl0601->calib_param.c00;
  h =  spl0601_read(HW_ADR, 0x15);
  m =  spl0601_read(HW_ADR, 0x16);
  l =  spl0601_read(HW_ADR, 0x17);
  p_spl0601->calib_param.c10 = (int32_t)h<<16 | (int32_t)m<<8 | l;
  p_spl0601->calib_param.c10 = (p_spl0601->calib_param.c10&0x080000)?(0xFFF00000|p_spl0601->calib_param.c10):p_spl0601->calib_param.c10;
  h =  spl0601_read(HW_ADR, 0x18);
  l  =  spl0601_read(HW_ADR, 0x19);
  p_spl0601->calib_param.c01 = (int16_t)h<<8 | l;
  h =  spl0601_read(HW_ADR, 0x1A);
  l  =  spl0601_read(HW_ADR, 0x1B);
  p_spl0601->calib_param.c11 = (int16_t)h<<8 | l;
  h =  spl0601_read(HW_ADR, 0x1C);
  l  =  spl0601_read(HW_ADR, 0x1D);
  p_spl0601->calib_param.c20 = (int16_t)h<<8 | l;
  h =  spl0601_read(HW_ADR, 0x1E);
  l  =  spl0601_read(HW_ADR, 0x1F);
  p_spl0601->calib_param.c21 = (int16_t)h<<8 | l;
  h =  spl0601_read(HW_ADR, 0x20);
  l  =  spl0601_read(HW_ADR, 0x21);
  p_spl0601->calib_param.c30 = (int16_t)h<<8 | l;
}


/*****************************************************************************
函 数 名  : spl0601_start_temperature
功能描述  : 发起一次温度测量
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_temperature(void)
{
  spl0601_write(HW_ADR, 0x08, 0x02);
}

/*****************************************************************************
函 数 名  : spl0601_start_pressure
功能描述  : 发起一次压力值测量
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_pressure(void)
{
  spl0601_write(HW_ADR, 0x08, 0x01);
}

/*****************************************************************************
函 数 名  : spl0601_start_continuous
功能描述  : Select node for the continuously measurement
输入参数  : uint8_t mode  1: pressure; 2: temperature; 3: pressure and temperature
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月25日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_start_continuous(uint8_t mode)
{
  spl0601_write(HW_ADR, 0x08, mode+4);
}


/*****************************************************************************
函 数 名  : spl0601_get_raw_temp
功能描述  : 获取温度的原始值，并转换成32Bits整数
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_temp(void)
{
  uint8_t h[3] = {0};
  h[0] = spl0601_read(HW_ADR, 0x03);
  h[1] = spl0601_read(HW_ADR, 0x04);
  h[2] = spl0601_read(HW_ADR, 0x05);
  p_spl0601->i32rawTemperature = (int32_t)h[0]<<16 | (int32_t)h[1]<<8 | (int32_t)h[2];
  p_spl0601->i32rawTemperature= (p_spl0601->i32rawTemperature&0x800000) ? (0xFF000000|p_spl0601->i32rawTemperature) : p_spl0601->i32rawTemperature;
}

/*****************************************************************************
函 数 名  : spl0601_get_raw_pressure
功能描述  : 获取压力原始值，并转换成32bits整数
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
void spl0601_get_raw_pressure(void)
{
  uint8_t h[3];
  h[0] = spl0601_read(HW_ADR, 0x00);
  h[1] = spl0601_read(HW_ADR, 0x01);
  h[2] = spl0601_read(HW_ADR, 0x02);
  p_spl0601->i32rawPressure = (int32_t)h[0]<<16 | (int32_t)h[1]<<8 | (int32_t)h[2];
  p_spl0601->i32rawPressure= (p_spl0601->i32rawPressure&0x800000) ? (0xFF000000|p_spl0601->i32rawPressure) : p_spl0601->i32rawPressure;
}


/*****************************************************************************
函 数 名  : spl0601_get_temperature
功能描述  : 在获取原始值的基础上，返回浮点校准后的温度值
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_temperature(void)
{
  float fTCompensate;
  float fTsc;
  
  fTsc = p_spl0601->i32rawTemperature / (float)p_spl0601->i32kT;
  fTCompensate =  p_spl0601->calib_param.c0 * 0.5 + p_spl0601->calib_param.c1 * fTsc;
  return fTCompensate;
}

/*****************************************************************************
函 数 名  : spl0601_get_pressure
功能描述  : 在获取原始值的基础上，返回浮点校准后的压力值
输入参数  : void
输出参数  : 无
返 回 值  :
调用函数  :
被调函数  :

修改历史      :
1.日    期   : 2015年11月30日
作    者   : WL
修改内容   : 新生成函数

*****************************************************************************/
float spl0601_get_pressure(void)
{
  float fTsc, fPsc;
  float qua2, qua3;
  float fPCompensate;
  
  fTsc = p_spl0601->i32rawTemperature / (float)p_spl0601->i32kT;
  fPsc = p_spl0601->i32rawPressure / (float)p_spl0601->i32kP;
  qua2 = p_spl0601->calib_param.c10 + fPsc * (p_spl0601->calib_param.c20 + fPsc* p_spl0601->calib_param.c30);
  qua3 = fTsc * fPsc * (p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21);
  //qua3 = 0.9f *fTsc * fPsc * (p_spl0601->calib_param.c11 + fPsc * p_spl0601->calib_param.c21);
  
  fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + fTsc * p_spl0601->calib_param.c01 + qua3;
  //fPCompensate = p_spl0601->calib_param.c00 + fPsc * qua2 + 0.9f *fTsc  * p_spl0601->calib_param.c01 + qua3;
  return fPCompensate;
}


void SPL06_Init(void)//气压计初始化配置
{
	spl0601_init();
  spl0601_rateset(PRESSURE_SENSOR,32, 16);//104.4ms
  spl0601_rateset(TEMPERATURE_SENSOR,128, 2);
  spl0601_start_continuous(CONTINUOUS_P_AND_T);//Start background measurement
}




float _temperature,_pressure;
uint8_t user_spl0601_get()//气压计数据获取状态机
{
  static uint16_t spl06_cnt=0;
  spl06_cnt++;
  if(spl06_cnt==1)//1
  {
    spl0601_get_raw_temp();
    _temperature = spl0601_get_temperature();
    return 0;
  }
  else  if(spl06_cnt>=20)
  {
    spl0601_get_raw_pressure();
    _pressure = spl0601_get_pressure();
    spl06_cnt=0;
    return 1;
  }
  return 0;
}


void SPL06_Read_Data(float *baro_t,float *baro_p)//气压计数据采集
{
  user_spl0601_get();
  *baro_p=_pressure;
  *baro_t=_temperature;
}

