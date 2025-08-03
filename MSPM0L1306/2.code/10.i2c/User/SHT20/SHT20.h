#ifndef __SHT20_H
#define __SHT20_H

#include <ti_msp_dl_config.h>

#define I2C_PORT (GPIOA)
#define I2C_SCL_PIN (DL_GPIO_PIN_0)
#define I2C_SCL_IOMUX (IOMUX_PINCM1)
#define I2C_SDA_PIN (DL_GPIO_PIN_1)
#define I2C_SDA_IOMUX (IOMUX_PINCM2)

// 设置SDA输出模式
#define SDA_OUT()   {                                                \
                        DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);    \
                        DL_GPIO_setPins(I2C_PORT, I2C_SDA_PIN);      \
                        DL_GPIO_enableOutput(I2C_PORT, I2C_SDA_PIN); \
                    }
// 设置SDA输入模式
#define SDA_IN()    { DL_GPIO_initDigitalInput(I2C_SDA_IOMUX); }
// 读取SDA电平
#define SDA_GET()   (((DL_GPIO_readPins(I2C_PORT,I2C_SDA_PIN) & I2C_SDA_PIN) > 0) ? 1 : 0)
// SDA与SCL电平控制
#define SDA(x)      ((x) ? (DL_GPIO_setPins(I2C_PORT,I2C_SDA_PIN)) : (DL_GPIO_clearPins(I2C_PORT,I2C_SDA_PIN)))
#define SCL(x)      ((x) ? (DL_GPIO_setPins(I2C_PORT,I2C_SCL_PIN)) : (DL_GPIO_clearPins(I2C_PORT,I2C_SCL_PIN)))

void I2C_GPIO_Init(void);
float SHT20_Read(uint8_t regaddr);

#endif 
