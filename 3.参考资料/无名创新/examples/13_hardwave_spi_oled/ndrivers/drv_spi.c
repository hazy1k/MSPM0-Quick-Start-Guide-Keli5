#include "ti_msp_dl_config.h"
#include "drv_spi.h"


uint8_t SPI_WriteByte(uint8_t Byte)
{
	while (DL_SPI_isBusy(SPI_0_INST));
	DL_SPI_transmitData8(SPI_0_INST, Byte);
	while(  DL_SPI_isRXFIFOEmpty(SPI_0_INST));
	//while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE));
	return DL_SPI_receiveData8(SPI_0_INST);
}


/****************SPI***************************/
/*------------дһ�����ݣ�Ƭѡ����-----------*/
void SPI_LCD_WrDat(unsigned char dat)
{
	OLED_CS_Clr();
  OLED_DC_Set();
  SPI_WriteByte(dat);//����8λ���ݣ�ʱ����������Ч
	OLED_CS_Set();
}


/*------------д���Ƭѡ����-------------*/
void SPI_LCD_WrCmd(unsigned char cmd)
{
	OLED_CS_Clr();
  OLED_DC_Clr();
	SPI_WriteByte(cmd);
  OLED_DC_Set();
	OLED_CS_Set();
}
