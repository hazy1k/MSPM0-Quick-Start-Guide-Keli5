#include "ti_msp_dl_config.h"
#include "systick.h"
#include "bsp_eeprom.h"



/******************************************************************************************/
//AT24C02的地址是256个字节，地址从0-255即 0x000 到 0xFF
uint8_t AT24CXX_Check(void);  //检查器件

#define AT24C01 127
#define AT24C02 255
#define AT24C04 511
#define AT24C08 1023
#define AT24C16 2047
#define AT24C32 4095
#define AT24C64 8191
#define AT24C128 16383
#define AT24C256 32767

#define EE_TYPE AT24C16//AT24C16
#define AT24C0X_IIC_ADDRESS  0xA0//1 0 1 0 A2 A1 A0 R/W

//IO操作函数
//#define SOFT_IIC_SCL_1  DL_GPIO_setPins(USER_GPIO_PORT,   USER_GPIO_EPM_SCL_PIN)
//#define SOFT_IIC_SCL_0  DL_GPIO_clearPins(USER_GPIO_PORT, USER_GPIO_EPM_SCL_PIN)
//#define SOFT_IIC_SDA_1  DL_GPIO_setPins(USER_GPIO_PORT,   USER_GPIO_EPM_SDA_PIN)
//#define SOFT_IIC_SDA_0  DL_GPIO_clearPins(USER_GPIO_PORT, USER_GPIO_EPM_SDA_PIN)
//#define SOFT_READ_SDA  (bool)(DL_GPIO_readPins(USER_GPIO_PORT, USER_GPIO_EPM_SDA_PIN)==USER_GPIO_EPM_SDA_PIN)//((DL_GPIO_readPins(USER_GPIO_PORT,  USER_GPIO_EPM_SDA_PIN))?:1,0)

#define SOFT_IIC_SCL_1 USER_GPIO_PORT->DOUTSET31_0 = USER_GPIO_EPM_SCL_PIN
#define SOFT_IIC_SCL_0 USER_GPIO_PORT->DOUTCLR31_0 = USER_GPIO_EPM_SCL_PIN
#define SOFT_IIC_SDA_1 USER_GPIO_PORT->DOUTSET31_0 = USER_GPIO_EPM_SDA_PIN
#define SOFT_IIC_SDA_0 USER_GPIO_PORT->DOUTCLR31_0 = USER_GPIO_EPM_SDA_PIN
#define SOFT_READ_SDA  (bool)((USER_GPIO_PORT->DIN31_0 & USER_GPIO_EPM_SDA_PIN)==USER_GPIO_EPM_SDA_PIN)


//IIC延时函数
static void SOFT_IIC_DLY(void)
{
  delay_us(5);
  //delay_cycles(CPUCLK_FREQ/100000);
}


//初始化IIC
static void IIC_Init(void)
{
  //DL_GPIO_enableHiZ(USER_GPIO_EPM_SCL_IOMUX);
  DL_GPIO_enableHiZ(USER_GPIO_EPM_SDA_IOMUX);

  DL_GPIO_setPins(USER_GPIO_PORT,   USER_GPIO_EPM_SCL_PIN);//设置高
  DL_GPIO_setPins(USER_GPIO_PORT,   USER_GPIO_EPM_SDA_PIN);//设置高
}

//IO方向设置

static void SOFT_SDA_OUT(void)
{
  DL_GPIO_initDigitalOutput(USER_GPIO_EPM_SDA_IOMUX);
  DL_GPIO_enableOutput(USER_GPIO_PORT, USER_GPIO_EPM_SDA_PIN);
}

static void SOFT_SDA_IN(void)
{
  DL_GPIO_initDigitalInputFeatures(USER_GPIO_EPM_SDA_IOMUX, DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
  DL_GPIO_enableHiZ(USER_GPIO_EPM_SDA_IOMUX);
}


//产生IIC起始信号
static void SOFT_IIC_Start(void)
{
  SOFT_SDA_OUT(); //sda线输出
  /*空闲状态*/
  SOFT_IIC_SDA_1;
  SOFT_IIC_SCL_1;
  /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
  delay_us(4);
  SOFT_IIC_SDA_0; //START:when CLK is high,DATA change form high to low
  delay_us(4);
  SOFT_IIC_SCL_0; //钳住I2C总线，准备发送或接收数据
  /* 时钟低电平，数据传输无效，持有SDA，钳住数据 */
}

//产生IIC停止信号
static void SOFT_IIC_Stop(void)
{
  SOFT_SDA_OUT(); //sda线输出
  SOFT_IIC_SCL_0;
  SOFT_IIC_SDA_0; //STOP:when CLK is high DATA change form low to high
  delay_us(4);
  SOFT_IIC_SCL_1;
  SOFT_IIC_SDA_1; //发送I2C总线结束信号
  delay_us(4);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static uint8_t SOFT_IIC_Wait_Ack(void)
{
  uint8_t ucErrTime = 0;
  SOFT_SDA_IN();//SDA设置为输入
  SOFT_IIC_SDA_1;
  delay_us(1);
  SOFT_IIC_SCL_1;//SCL=1, 此时从机可以返回ACK
  delay_us(1);

  while (SOFT_READ_SDA)
  {
    ucErrTime++;

    if (ucErrTime > 250)
    {
      SOFT_IIC_Stop();
      return 1;
    }
  }

  SOFT_IIC_SCL_0; //时钟输出0
  return 0;
}


//产生ACK应答
static void SOFT_IIC_Ack(void)
{
  SOFT_IIC_SCL_0;
  SOFT_SDA_OUT();
  SOFT_IIC_SDA_0;
  delay_us(2);
  SOFT_IIC_SCL_1;
  delay_us(2);
  SOFT_IIC_SCL_0;
}

//不产生ACK应答
static void SOFT_IIC_NAck(void)
{
  SOFT_IIC_SCL_0;
  SOFT_SDA_OUT();
  SOFT_IIC_SDA_1;
  delay_us(2);
  SOFT_IIC_SCL_1;
  delay_us(2);
  SOFT_IIC_SCL_0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
static void SOFT_IIC_Send_Byte(uint8_t txd)
{
  uint8_t t;
  SOFT_SDA_OUT();
  SOFT_IIC_SCL_0; //拉低时钟开始数据传输

  for (t = 0; t < 8; t++)
  {
    //时钟线SCL拉低电平期间，才允许数据线SDA电平变化
    if((txd & 0x80) >> 7)  SOFT_IIC_SDA_1;
    else SOFT_IIC_SDA_0;

    txd <<= 1;//移位操作，先发送高位
    delay_us(2);
    SOFT_IIC_SCL_1;//时钟线SCL拉高，上升延触发SDA数据位发送
    delay_us(2);
    SOFT_IIC_SCL_0;
    delay_us(2);
  }
}


//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
static uint8_t SOFT_IIC_Read_Byte(unsigned char ack)
{
  unsigned char i, receive = 0;
  SOFT_SDA_IN(); //SDA设置为输入

  for (i = 0; i < 8; i++)
  {
    SOFT_IIC_SCL_0;
    delay_us(2);
    SOFT_IIC_SCL_1;
    receive <<= 1;

    if (SOFT_READ_SDA)
      receive++;

    delay_us(1);
  }

  if (!ack)
    SOFT_IIC_NAck(); //发送nACK
  else
    SOFT_IIC_Ack(); //发送ACK

  return receive;
}



//初始化IIC接口
void AT24CXX_Init(void)
{
  IIC_Init();

  while(AT24CXX_Check()) //检测不到24c02
  {
    delay_ms(20);
  }

  //AT24CXX_Erase_All();
}


//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{
  uint8_t temp = 0;
  SOFT_IIC_Start();

  if (EE_TYPE > AT24C16)
  {
    SOFT_IIC_Send_Byte(AT24C0X_IIC_ADDRESS); //发送写命令
    SOFT_IIC_Wait_Ack();
    SOFT_IIC_Send_Byte(ReadAddr >> 8); //发送高地址
    //SOFT_IIC_Wait_Ack();
  }
  else    SOFT_IIC_Send_Byte(AT24C0X_IIC_ADDRESS + ((ReadAddr / 256) << 1)); //发送器件地址0XA0,写数据

  SOFT_IIC_Wait_Ack();
  SOFT_IIC_Send_Byte(ReadAddr % 256); //发送低地址
  SOFT_IIC_Wait_Ack();

  SOFT_IIC_Start();
  SOFT_IIC_Send_Byte(AT24C0X_IIC_ADDRESS + 1); //进入接收模式
  SOFT_IIC_Wait_Ack();
  temp = SOFT_IIC_Read_Byte(0);
  SOFT_IIC_Stop();//产生一个停止条件
  return temp;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
  SOFT_IIC_Start();

  if (EE_TYPE > AT24C16)
  {
    SOFT_IIC_Send_Byte(AT24C0X_IIC_ADDRESS); //发送写命令
    SOFT_IIC_Wait_Ack();
    SOFT_IIC_Send_Byte(WriteAddr >> 8); //发送高地址
  }
  else
  {
    SOFT_IIC_Send_Byte(AT24C0X_IIC_ADDRESS + ((WriteAddr / 256) << 1)); //发送器件地址0XA0,写数据
  }

  SOFT_IIC_Wait_Ack();
  SOFT_IIC_Send_Byte(WriteAddr % 256); //发送低地址
  SOFT_IIC_Wait_Ack();
  SOFT_IIC_Send_Byte(DataToWrite); //发送字节
  SOFT_IIC_Wait_Ack();
  SOFT_IIC_Stop(); //产生一个停止条件
  delay_ms(10);//10
}



void AT24CXX_Erase_All(void)
{
  for(uint16_t i = 0; i <= EE_TYPE; i++)
  {
    AT24CXX_WriteOneByte(i, 0xff);
    delay_ms(10);
  }
}

uint8_t at24cxx_debug[2048];
void AT24CXX_Read_All(void)
{
  for(uint16_t i = 0; i <= EE_TYPE; i++)
  {
    at24cxx_debug[i] = AT24CXX_ReadOneByte(i);
  }
}

//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(uint16_t WriteAddr, u32 DataToWrite, uint8_t Len)
{
  uint8_t t;

  for (t = 0; t < Len; t++)
  {
    AT24CXX_WriteOneByte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
  }
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 AT24CXX_ReadLenByte(uint16_t ReadAddr, uint8_t Len)
{
  uint8_t t;
  u32 temp = 0;

  for (t = 0; t < Len; t++)
  {
    temp <<= 8;
    temp += AT24CXX_ReadOneByte(ReadAddr + Len - t - 1);
  }

  return temp;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead)
{
  while (NumToRead)
  {
    *pBuffer++ = AT24CXX_ReadOneByte(ReadAddr++);
    NumToRead--;
  }
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
  while (NumToWrite--)
  {
    AT24CXX_WriteOneByte(WriteAddr, *pBuffer);
    WriteAddr++;
    pBuffer++;
  }
}

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
#define at24cx_check_address 1600//1600  240
#define at24cx_end_address 	 2047//2047  255 
uint8_t AT24CXX_Check(void)
{
  uint8_t temp;
  temp = AT24CXX_ReadOneByte(at24cx_end_address); //避免每次开机都写AT24CXX

  if (temp == 0X55)	return 0;
  else //排除第一次初始化的情况
  {
    //AT24CXX_Erase_All();//先全部擦除
    AT24CXX_WriteOneByte(at24cx_end_address, 0X55);
    temp = AT24CXX_ReadOneByte(at24cx_end_address);

    if (temp == 0X55)	return 0;
  }

  return 1;
}


typedef union
{
  unsigned char Bdata[4];
  float Fdata;
  uint32_t Idata;
} FBI;

static FBI data;


void EEPROMRead(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    AT24CXX_Read(ui32Address + 4 * i, data.Bdata, 4);
    *pui32Data = data.Idata;
    pui32Data++;
  }
}


uint32_t EEPROMProgram(uint32_t *pui32Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    data.Idata = *pui32Data;
    AT24CXX_Write(ui32Address + 4 * i, data.Bdata, 4);
    pui32Data++;
  }

  return 1;
}


union
{
  unsigned char floatByte[4];
  float floatValue;
} _FloatUnion;

/***************************************************************************************
@函数名：void Float2Byte(float *FloatValue, unsigned char *Byte, unsigned char Subscript)
@入口参数：FloatValue:float值
			     Byte:数组
		       Subscript:指定从数组第几个元素开始写入
@出口参数：无
功能描述：将float数据转成4字节数据并存入指定地址
@作者：无名小哥
@日期：2020年01月17日
****************************************************************************************/
void _Float2Byte(float *FloatValue, volatile unsigned char *Byte, unsigned char Subscript)
{
  _FloatUnion.floatValue = (float)2;

  if(_FloatUnion.floatByte[0] == 0)//小端模式
  {
    _FloatUnion.floatValue = *FloatValue;
    Byte[Subscript]     = _FloatUnion.floatByte[0];
    Byte[Subscript + 1] = _FloatUnion.floatByte[1];
    Byte[Subscript + 2] = _FloatUnion.floatByte[2];
    Byte[Subscript + 3] = _FloatUnion.floatByte[3];
  }
  else//大端模式
  {
    _FloatUnion.floatValue = *FloatValue;
    Byte[Subscript]     = _FloatUnion.floatByte[3];
    Byte[Subscript + 1] = _FloatUnion.floatByte[2];
    Byte[Subscript + 2] = _FloatUnion.floatByte[1];
    Byte[Subscript + 3] = _FloatUnion.floatByte[0];
  }
}


/***************************************************************************************
@函数名：void Byte2Float(unsigned char *Byte, unsigned char Subscript, float *FloatValue)
@入口参数：Byte:数组
			     Subscript:指定从数组第几个元素开始写入
		       FloatValue:float值
@出口参数：无
功能描述：从指定地址将4字节数据转成float数据
@作者：无名小哥
@日期：2020年01月17日
****************************************************************************************/
void _Byte2Float(volatile unsigned char *Byte, unsigned char Subscript, float *FloatValue)
{
  _FloatUnion.floatByte[0] = Byte[Subscript];
  _FloatUnion.floatByte[1] = Byte[Subscript + 1];
  _FloatUnion.floatByte[2] = Byte[Subscript + 2];
  _FloatUnion.floatByte[3] = Byte[Subscript + 3];
  *FloatValue = _FloatUnion.floatValue;
}


volatile unsigned char _byte[4];

void EEPROMRead_f(float *Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    //AT24CXX_Read(ui32Address+4*i,(unsigned char *)_byte,4);
    _byte[0] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 0);
    _byte[1] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 1);
    _byte[2] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 2);
    _byte[3] = AT24CXX_ReadOneByte(ui32Address + 4 * i + 3);
    _Byte2Float(_byte, 0, Data);
    Data++;
  }
}

uint32_t EEPROMProgram_f(float *Data, uint32_t ui32Address, uint32_t ui32Count)
{
  for(uint16_t i = 0; i < ui32Count; i++)
  {
    _Float2Byte(Data, _byte, 0);
    //AT24CXX_Write(ui32Address+4*i,(unsigned char *)_byte,4);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 0, _byte[0]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 1, _byte[1]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 2, _byte[2]);
    delay_ms(50);
    AT24CXX_WriteOneByte(ui32Address + 4 * i + 3, _byte[3]);
    delay_ms(50);
    Data++;
  }
  return 1;
}



void bsp_eeprom_init(void)
{
  AT24CXX_Init();
  //AT24CXX_Read_All();
}






#define WP_FLASH_BASE 0
static float eeprom_write_data[3] = {0, 0, 0};
void ReadFlashParameterALL(FLIGHT_PARAMETER *WriteData)
{
  #if eeprom_mode==0
	EEPROMRead((uint32_t *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
	#else
	W25QXX_Read_f((float *)(&WriteData->parameter_table), WP_FLASH_BASE, FLIGHT_PARAMETER_TABLE_NUM);
  #endif
}

void ReadFlashParameterOne(uint16_t Label, float *ReadData)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
	#else
	W25QXX_Read_f((float *)(ReadData), WP_FLASH_BASE + 4 * Label, 1);
  #endif
}

void ReadFlashParameterTwo(uint16_t Label, float *ReadData1, float *ReadData2)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead((uint32_t *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
	#else
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);	
  #endif
}

void ReadFlashParameterThree(uint16_t Label, float *ReadData1, float *ReadData2, float *ReadData3)
{
  #if eeprom_mode==0
  EEPROMRead((uint32_t *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead((uint32_t *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  EEPROMRead((uint32_t *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #elif eeprom_mode==1
  EEPROMRead_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  EEPROMRead_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  EEPROMRead_f((float *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #else
  W25QXX_Read_f((float *)(ReadData1), WP_FLASH_BASE + 4 * Label, 1);;
  W25QXX_Read_f((float *)(ReadData2), WP_FLASH_BASE + 4 * Label + 4, 1);
  W25QXX_Read_f((float *)(ReadData3), WP_FLASH_BASE + 4 * Label + 8, 1);
  #endif
}

void WriteFlashParameter(uint16_t Label,
                         float WriteData)
{
  eeprom_write_data[0] = WriteData; //将需要更改的字段赋新
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 1);
  #endif
}



void WriteFlashParameter_Two(uint16_t Label,
                             float WriteData1,
                             float WriteData2)
{
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新=WriteData1;//将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新=WriteData2;//将需要更改的字段赋新值
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 2);
  #endif
}

void WriteFlashParameter_Three(uint16_t Label,
                               float WriteData1,
                               float WriteData2,
                               float WriteData3)
{
  eeprom_write_data[0] = WriteData1; //将需要更改的字段赋新值
  eeprom_write_data[1] = WriteData2; //将需要更改的字段赋新值
  eeprom_write_data[2] = WriteData3; //将需要更改的字段赋新值
  #if eeprom_mode==0
  EEPROMProgram((uint32_t *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
  #elif eeprom_mode==1
  EEPROMProgram_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
	#else
	W25QXX_Write_f((float *)(&eeprom_write_data[0]), WP_FLASH_BASE + 4 * Label, 3);
  #endif
}



FLIGHT_PARAMETER Flight_Params =
{
  .num = FLIGHT_PARAMETER_TABLE_NUM
};

void flight_read_flash_full(void)
{
  ReadFlashParameterALL((FLIGHT_PARAMETER *)(&Flight_Params));

  for(uint16_t i = 0; i < Flight_Params.num; i++)
  {
    if(isnan(Flight_Params.parameter_table[i]) == 0)
      Flight_Params.health[i] = true;
  }
}