#ifndef __LCD_INIT_H
#define __LCD_INIT_H


#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 320
#define LCD_H 480

#else
#define LCD_W 480
#define LCD_H 320
#endif




//-----------------LCD端口定义---------------- 


#define LCD_RES_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_RST_PIN)//RES
#define LCD_RES_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_RST_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(PORTB_PORT, PORTB_DC_PIN)//DC
#define LCD_DC_Set()   DL_GPIO_setPins(PORTB_PORT, PORTB_DC_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_CS_PIN)//BLK
#define LCD_BLK_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_CS_PIN)



void LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节
void LCD_WR_REG(uint8_t dat);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化


#endif




