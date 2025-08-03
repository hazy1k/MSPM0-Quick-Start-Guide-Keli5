#ifndef __W25Q32_H__
#define __W25Q32_H__

#include <ti_msp_dl_config.h>

/* SPI接口相关参数宏定义 */
#define SPIx            SPI1
#define SPI_PICO_PORT   GPIOB
#define SPI_PICO_PIN    DL_GPIO_PIN_15
#define SPI_IOMUX_PICO  (IOMUX_PINCM32)
#define SPI_PICO_FUNC   IOMUX_PINCM32_PF_SPI1_PICO
#define SPI_POCI_PORT   GPIOB
#define SPI_POCI_PIN    DL_GPIO_PIN_14
#define SPI_IOMUX_POCI  (IOMUX_PINCM31)
#define SPI_POCI_FUNC   IOMUX_PINCM31_PF_SPI1_POCI
#define SPI_SCLK_PORT   GPIOB
#define SPI_SCLK_PIN    DL_GPIO_PIN_16
#define SPI_IOMUX_SCLK  (IOMUX_PINCM33)
#define SPI_SCLK_FUNC   IOMUX_PINCM33_PF_SPI1_SCLK
#define SPI_CS1_PORT    GPIOA
#define SPI_CS1_PIN     DL_GPIO_PIN_27
#define SPI_IOMUX_CS1   (IOMUX_PINCM60)
#define SPI_CS1_FUNC    IOMUX_PINCM60_PF_SPI1_CS1_POCI1
#define CS_PORT         GPIOB
#define CS_PIN_PIN      DL_GPIO_PIN_17
#define CS_PIN_IOMUX    (IOMUX_PINCM43)

/* CS控制宏定义 */
#define SPI_CS(x)  ((x)?DL_GPIO_setPins(CS_PORT,CS_PIN_PIN):DL_GPIO_clearPins(CS_PORT,CS_PIN_PIN))

void SPIx_Init(void);
uint16_t W25Q32_readID(void);
void W25Q32_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);      
void W25Q32_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length);

#endif
