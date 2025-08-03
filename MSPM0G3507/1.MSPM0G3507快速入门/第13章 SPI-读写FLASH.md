# 第十三章 SPI-读写FLASH

## 1. 硬件设计

### 1.1 SPI简介

SPI主要使用4根线，时钟线（SCLK），主输出从输入线（MOSI），主输入从输出线（MISO）和片选线（CS）。

| **通信线** | **说明**                                                       |
| ------- | ------------------------------------------------------------ |
| SCLK    | 时钟线，也叫做SCK。由主机产生时钟信号。                                        |
| MOSI    | 主设备输出从设备输入线，也叫做SDO。意为主机向从机发送数据。                              |
| MISO    | 主设备输入从设备输出线，也叫做SDI。意为主机接收从机的数据。                              |
| CS      | 片选线，也叫做NSS。从机使能信号，由主机控制。当我们的主机控制某个从机时，需要将从机对应的片选引脚电平拉低或者是拉高。 |

主设备是通过片选线选择要与之通信的从设备。每个从设备都有一个片选线，当片选线为低电平时，表示该从设备被选中。（也有一些设备以高电平有效，需要根据其数据手册确定）。主设备通过控制时钟线的电平来同步数据传输。时钟线的上升沿和下降沿用于控制数据的传输和采样。SPI的主从接线方式需要对应，主从机设定后身份固定。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/spi/spi_20240708_144647.png)

### 1.2 W25Q32介绍

W25Q32是一种常见的串行闪存器件，它采用SPI（Serial Peripheral Interface）接口协议，具有高速读写和擦除功能，可用于存储和读取数据。W25Q32芯片容量为32 Mbit（4 MB），其中名称后的数字代表不同的容量选项。不同的型号和容量选项可以满足不同应用的需求，比如W25Q16、W25Q64、W25Q128等。通常被用于嵌入式设备、存储设备、路由器等高性能电子设备中。 W25Q32闪存芯片的内存分配是按照扇区（Sector）和块（Block）进行的，每个扇区的大小为4KB，每个块包含16个扇区，即一个块的大小为64KB。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/spi/spi_20240708_145156.png)

W25Q32存储芯片，其引脚的说明，见下表。

| CLK  | 从外部获取时间，为输入输出功能提供时钟                                                            |
| ---- | ------------------------------------------------------------------------------ |
| DI   | 标准SPI使用单向的DI，来串行的写入指令，地址，或者数据到FLASH中，在时钟的上升沿。                                  |
| DO   | 标准SPI使用单向的DO，来从处于下降边沿时钟的设备，读取数据或者状态。                                           |
| WP   | 防止状态寄存器被写入                                                                     |
| HOLD | 当它有效时允许设备暂停，低电平：DO引脚高阻态，DI CLK引脚的信号被忽略。高电平：设备重新开始，当多个设备共享相同的SPI信 号的时候该功能可能会被用到 |
| CS   | CS高电平的时候其他引脚成高阻态，处于低电平的时候，可以读写数据                                               |

它与开发板的连接如下：

| 开发板（主机） | W25Q32（从机）    | 说明        |
| ------- | ------------- | --------- |
| PB17    | CS(NSS)       | 片选线       |
| ---     | ---           | ---       |
| PB16    | CLK           | 时钟线       |
| PB14    | DO(IO1)(MISO) | 主机输入从机输出线 |
| PB15    | DI(IO0)(MOSI) | 主机输出从机输入线 |
| GND     | GND           | 电源线       |
| VCC     | 3V3           | 电源线       |

开发板的背面默认已经为大家贴好了该存储芯片，大家只需要了解连接的是哪一个引脚即可。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/spi/spi_20240708_145313.png)

需要注意的是，我们使用的是硬件SPI方式驱动W25Q32，因此我们需要确定我们设置的引脚是否有硬件SPI外设接口。在数据手册中，PB14~PB17可以复用为SPI1的4根通信线。

![](https://wiki.lckfb.com/storage/images/zh-hans/dmx/beginner/spi/spi_20240708_145339.png)

这里需要注意的是PICO，表示的是外设输入控制器输出，即对应SPI中的MOSI。POCI表示外设输出控制器输入，即对应MISO。

## 2. 软件设计

### 2.1 编程大纲

1. SPI接口相关参数宏定义

2. SPI接口初始化

3. W25Q32基础函数

4. 主函数测试读写

### 2.2 代码分析

#### 2.2.1 SPI相关参数宏定义

```c
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
```

#### 2.2.2 SPI接口初始化

```c
void SPI_GPIO_init(void)
{
    DL_GPIO_initPeripheralOutputFunction(SPI_IOMUX_SCLK, SPI_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(SPI_IOMUX_PICO, SPI_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(SPI_IOMUX_POCI, SPI_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(SPI_IOMUX_CS1, SPI_CS1_FUNC);
    DL_GPIO_initDigitalOutput(CS_PIN_IOMUX);
    DL_GPIO_clearPins(CS_PORT, CS_PIN_PIN);
    DL_GPIO_enableOutput(CS_PORT, CS_PIN_PIN);
}

static const DL_SPI_Config gSPI_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,             // 主机模式
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,// MOTO4模式
    .parity      = DL_SPI_PARITY_NONE,                 // 无校验位
    .dataSize    = DL_SPI_DATA_SIZE_8,                 // 8位数据位
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,         // MSB 先发
    .chipSelectPin = DL_SPI_CHIP_SELECT_1,             // 片选引脚为 CS1
};

static const DL_SPI_ClockConfig gSPI_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,               // 时钟源为总线时钟
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1        // 时钟分频系数为 1
};

void SPIx_init(void) 
{
    DL_SPI_setClockConfig(SPIx, (DL_SPI_ClockConfig*)&gSPI_clockConfig); // 时钟配置
    DL_SPI_init(SPIx, (DL_SPI_Config *) &gSPI_config); // SPI 配置
    DL_SPI_setBitRateSerialClockDivider(SPIx, 0); 
    DL_SPI_setFIFOThreshold(SPIx, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);
    DL_SPI_enable(SPIx);
}

void SPIx_Init(void)
{
    SPI_GPIO_init();
    SPIx_init();
}
```

#### 2.2.3 W25Q32基本函数

##### 2.2.3.1 通过SPI读写一个字节

```c
uint8_t spi_read_write_byte(uint8_t dat) // SPI读写一个字节函数 
{
        uint8_t data = 0;
        DL_SPI_transmitData8(SPIx,dat);
        while(DL_SPI_isBusy(SPIx));
        data = DL_SPI_receiveData8(SPIx);
        while(DL_SPI_isBusy(SPIx));
        return data;
}
```

##### 2.2.3.2 写使能与等待空闲

```c
void W25Q32_write_enable(void) // SPI写使能
{
    SPI_CS(0);                           
    spi_read_write_byte(0x06);                  
    SPI_CS(1); 
}                          

void W25Q32_wait_busy(void) // SPI等待空闲
{   
    unsigned char byte = 0;
    do{ 
        SPI_CS(0);                             
        spi_read_write_byte(0x05);                
        byte = spi_read_write_byte(0Xff);       
        SPI_CS(1);      
     }while((byte&0x01 ) == 1);  
}
```

##### 2.2.3.3 读取设备ID

```c
uint16_t W25Q32_readID(void) // SPI读取ID
{
    uint16_t temp = 0;          
    SPI_CS(0);
    W25Q32_write_enable(); // 写使能    
    W25Q32_wait_busy(); // 等待空闲    
    // 发送读取 ID 的命令
    spi_read_write_byte(0x90);  // 0x90 是读取设备 ID 的 SPI 命令
    // 发送三个地址字节，这里用 0x00 0x00 0x00
    spi_read_write_byte(0x00);  // 地址字节 1
    spi_read_write_byte(0x00);  // 地址字节 2
    spi_read_write_byte(0x00);  // 地址字节 3
    // 读取返回的设备 ID，前一个字节为高字节，后一个字节为低字节
    temp = spi_read_write_byte(0xFF) << 8;  // 读取高字节
    temp |= spi_read_write_byte(0xFF);      // 读取低字节
    SPI_CS(1);  // 释放 SPI 总线
    return temp;
}
```

##### 2.2.3.4 擦除扇区

```c
void W25Q32_erase_sector(uint32_t addr) // 擦除一个扇区
{
    addr *= 4096;
    W25Q32_write_enable();  
    W25Q32_wait_busy();     
    SPI_CS(0);                                      
    spi_read_write_byte(0x20);         
    spi_read_write_byte((uint8_t)((addr)>>16));      
    spi_read_write_byte((uint8_t)((addr)>>8));   
    spi_read_write_byte((uint8_t)addr);
    SPI_CS(1);                                                                 
    W25Q32_wait_busy();   
}   
```

##### 2.2.3.5 读写数据

```c
// W25Q32写入数据
void W25Q32_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte)
{    
    unsigned int i = 0;
    W25Q32_erase_sector(addr/4096);
    W25Q32_write_enable();  
    W25Q32_wait_busy();    
    SPI_CS(0);                                    
    spi_read_write_byte(0x02);      
    spi_read_write_byte((uint8_t)((addr)>>16));  
    spi_read_write_byte((uint8_t)((addr)>>8));   
    spi_read_write_byte((uint8_t)addr);   
    for(i=0;i<numbyte;i++)
    {
        spi_read_write_byte(buffer[i]);  
    }
    SPI_CS(0);
    W25Q32_wait_busy();      
}

// W25Q32读取数据
void W25Q32_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length)   
{ 
    uint16_t i;                   
    SPI_CS(0);         
    spi_read_write_byte(0x03);                          
    spi_read_write_byte((uint8_t)((read_addr)>>16));       
    spi_read_write_byte((uint8_t)((read_addr)>>8));   
    spi_read_write_byte((uint8_t)read_addr);   
    for(i=0;i<read_length;i++)
    { 
        buffer[i]= spi_read_write_byte(0XFF);  
    }
    SPI_CS(1);                                    
} 
```

#### 2.2.4 主函数测试读写

```c
#include <ti_msp_dl_config.h>
#include "SysTick.h"
#include "Uart.h"
#include "W25Q32.h"
#include <stdio.h>

int main(void)
{
    SYSCFG_DL_init();
    UART_0_init();
    SysTick_Init();
    SPIx_Init();
    uint8_t buff[10] = {0};        
    W25Q32_read(buff, 0, 5);
    printf("buff = %s\r\n",buff);                
    W25Q32_write((uint8_t*)"6666", 0, 5);
    delay_ms(100);
    W25Q32_read(buff, 0, 5);
    printf("buff = %s\r\n",buff);
    while (1) 
    {        
    }
}
```

## 3. 小结

写入数据步骤如图。在FLASH存储器中，每次写入数据都要确保其中的数据为0xFF，是因为FLASH存储器的写入操作是一种擦除-写入操作。擦除操作是将存储单元中的数据全部置为1，也就是0xFF。然后，只有将要写入的数据位为0的位置才能进行写入操作，将其改变为0。这个过程是不可逆的，所以在写入数据之前，需要先确保要写入的位置为0xFF，然后再写入数据。

这种擦除-写入的操作是由于FLASH存储器的特殊结构决定的。FLASH存储器中的存储单元是通过电子门的状态进行控制的，每个门可以存储一个二进制位。擦除操作需要将门的状态恢复为初始状态，即全部为1。然后通过改变门的状态，将需要存储的数据位改变为0。所以在写入数据之前，需要确保存储单元的状态为1，以便进行正确的写入操作。

另外，FLASH存储器的擦除操作是以块为单位进行的，而不是单个存储单元。所以如果要写入数据的位置上已经有数据存在，需要进行擦除操作，将整个块的数据都置为1，然后再写入新的数据。这也是为什么在FLASH写入数据之前需要确保其中的数据为0xFF的原因。

![](https://wiki.lckfb.com/storage/images/zh-hans/dwx-cw32f030c8t6/beginner/spi/spi_20240628_164313.png)
