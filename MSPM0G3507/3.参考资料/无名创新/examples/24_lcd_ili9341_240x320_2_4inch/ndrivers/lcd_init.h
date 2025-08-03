#ifndef __LCD_INIT_H
#define __LCD_INIT_H


#define USE_HORIZONTAL 0  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 320
#define LCD_H 480

#else
#define LCD_W 480
#define LCD_H 320
#endif




//-----------------LCD�˿ڶ���---------------- 


#define LCD_RES_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_RST_PIN)//RES
#define LCD_RES_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_RST_PIN)

#define LCD_DC_Clr()   DL_GPIO_clearPins(PORTB_PORT, PORTB_DC_PIN)//DC
#define LCD_DC_Set()   DL_GPIO_setPins(PORTB_PORT, PORTB_DC_PIN)

#define LCD_BLK_Clr()  DL_GPIO_clearPins(PORTB_PORT, PORTB_CS_PIN)//BLK
#define LCD_BLK_Set()  DL_GPIO_setPins(PORTB_PORT, PORTB_CS_PIN)



void LCD_Writ_Bus(uint8_t dat);//ģ��SPIʱ��
void LCD_WR_DATA8(uint8_t dat);//д��һ���ֽ�
void LCD_WR_DATA(uint16_t dat);//д�������ֽ�
void LCD_WR_REG(uint8_t dat);//д��һ��ָ��
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��


#endif




