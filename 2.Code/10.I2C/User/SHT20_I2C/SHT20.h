#ifndef __SHT20_H
#define __SHT20_H

#include "ti_msp_dl_config.h"
#include "Uart.h"
#include "stdio.h"

/* I2C GPIO配置 */
#define I2C_PORT GPIOA
#define I2C_SCL_PIN DL_GPIO_PIN_0
#define I2C_SCL_IOMUX IOMUX_PINCM1
#define I2C_SDA_PIN DL_GPIO_PIN_1
#define I2C_SDA_IOMUX IOMUX_PINCM2

/* I2C模式配置 */
// 设置SDA输出模式
#define I2C_SDA_OUT() {DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX); \
                       DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN); \
                       DL_GPIO_enableOutput(I2C_PORT, I2C_SDA_PIN);}
// 设置SDA输入模式
#define I2C_SDA_IN()  {DL_GPIO_initDigitalInput(I2C_SDA_IOMUX);}
// 获取SDA的电平，并判断高低电平
#define I2C_SDA_GET() (((DL_GPIO_readPins(I2C_PORT, I2C_SDA_PIN)&I2C_SDA_PIN)>0)?1:0)
// 设置SDA的电平
#define SDA(x) ((x)?(DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN)):(DL_GPIO_clearPins(I2C_PORT, I2C_SDA_PIN))) 
// 设置SCL的电平
#define SCL(x) ((x)?(DL_GPIO_setPins(I2C_PORT, I2C_SCL_PIN)):(DL_GPIO_clearPins(I2C_PORT, I2C_SCL_PIN))) 

void I2C_GPIO_Init(void);
uint16_t SHT20_Read(uint8_t regaddr);

#endif 
