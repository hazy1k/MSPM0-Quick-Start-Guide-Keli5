#include "W25Q32.h"
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

uint8_t spi_read_write_byte(uint8_t dat) // SPI读写一个字节函数 
{
        uint8_t data = 0;
        DL_SPI_transmitData8(SPIx,dat);
        while(DL_SPI_isBusy(SPIx));
        data = DL_SPI_receiveData8(SPIx);
        while(DL_SPI_isBusy(SPIx));
        return data;
}

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
