#include "ti_msp_dl_config.h"
#include "systick.h"
#include "drv_spi.h"


//uint8_t SPI_WriteByte(uint8_t Byte)
//{
//	DL_SPI_transmitData8(SPI_0_INST, Byte);
//	while (DL_SPI_isBusy(SPI_0_INST));
//	return DL_SPI_receiveData8(SPI_0_INST);
//}

//uint16_t spi_retry_cnt = 0;
//uint8_t SPI_WriteByte(uint8_t Byte)
//{
//  spi_retry_cnt=0;
//	DL_SPI_transmitData8(SPI_0_INST, Byte);  //发送1字节命令
//	while(DL_SPI_isBusy(SPI_0_INST))        //等待发送完成
//	{
//    spi_retry_cnt++;
//    if(spi_retry_cnt > 200)	return 0;//超时     
//	}
//	return DL_SPI_receiveData8(SPI_0_INST);  //清空接收FIFO中的数据
//}


//#define SPI_CLK0   DL_GPIO_clearPins(PORTA_PORT,   PORTA_W25Q_SCLK_PIN)
//#define SPI_CLK1   DL_GPIO_setPins(PORTA_PORT,     PORTA_W25Q_SCLK_PIN)
//#define SPI_MOSI0  DL_GPIO_clearPins(PORTB_PORT,   PORTB_W25Q_MOSI_PIN)
//#define SPI_MOSI1  DL_GPIO_setPins(PORTB_PORT,     PORTB_W25Q_MOSI_PIN)

#define SPI_CLK1  PORTA_PORT->DOUTSET31_0 = PORTA_W25Q_SCLK_PIN
#define SPI_CLK0  PORTA_PORT->DOUTCLR31_0 = PORTA_W25Q_SCLK_PIN
#define SPI_MOSI1 PORTB_PORT->DOUTSET31_0 = PORTB_W25Q_MOSI_PIN
#define SPI_MOSI0 PORTB_PORT->DOUTCLR31_0 = PORTB_W25Q_MOSI_PIN
	



void SPI_WriteByte(unsigned char data_in)
{
  unsigned char data = 0x00;
  for(uint8_t i = 0; i < 8; i++)
  {	
		SPI_CLK0;
		if((data_in&(0x80 >> i))!=0) SPI_MOSI1;
		else SPI_MOSI0;
    SPI_CLK1;
  }
	SPI_CLK0;
}



/****************SPI***************************/
/*------------写一个数据：片选拉高-----------*/
void SPI_LCD_WrDat(unsigned char dat)
{
	OLED_CS_Clr();
  OLED_DC_Set();
  SPI_WriteByte(dat);//传送8位数据：时钟线拉低有效
	OLED_CS_Set();
}


/*------------写命令：片选拉低-------------*/
void SPI_LCD_WrCmd(unsigned char cmd)
{
	OLED_CS_Clr();
  OLED_DC_Clr();
	SPI_WriteByte(cmd);
  OLED_DC_Set();
	OLED_CS_Set();
}
