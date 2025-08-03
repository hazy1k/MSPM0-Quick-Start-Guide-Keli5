#ifndef drv_spi_h
#define drv_spi_h



#define OLED_RST_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_RST_PIN)
#define OLED_RST_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_RST_PIN)

#define OLED_DC_Clr() DL_GPIO_clearPins(PORTB_PORT, PORTB_DC_PIN)
#define OLED_DC_Set() DL_GPIO_setPins(PORTB_PORT, PORTB_DC_PIN)


#define OLED_CS_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_CS_PIN)
#define OLED_CS_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_CS_PIN)

void SPI_WriteByte(uint8_t Byte);
void SPI_LCD_WrDat(unsigned char dat);
void SPI_LCD_WrCmd(unsigned char cmd);


#endif




