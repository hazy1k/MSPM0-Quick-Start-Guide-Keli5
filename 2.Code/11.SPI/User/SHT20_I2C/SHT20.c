#include "SHT20.h"

void I2C_GPIO_Init(void)
{
    // 初始化数字输出引脚
    DL_GPIO_initDigitalOutput(I2C_SCL_IOMUX);
    DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);
    DL_GPIO_setPins(I2C_PORT, I2C_SCL_PIN|I2C_SDA_PIN); // 拉高I2C总线
    DL_GPIO_enableOutput(I2C_PORT, I2C_SCL_PIN|I2C_SDA_PIN); // 使能I2C总线输出
}

void I2C_Start(void) // 产生I2C起始信号
{
    I2C_SDA_OUT();
    SCL(0);
    SDA(1);
    SCL(1);
    SDA(0);
    SCL(0);
}

void I2C_Stop(void) // 产生I2C停止信号
{
    I2C_SDA_OUT();
    SCL(0);
    SDA(0);
    SCL(1);
    SDA(1);
}

void I2C_Send_Ack(uint8_t ack) // 主机发送应答信号
{
    I2C_SDA_OUT();
    SCL(0);
    SDA(0);
    if(!ack)
    {
        SDA(0); // 应答为0，主机发送ACK
    }
    else
    {
        SDA(1); // 应答为1，主机发送NAK
    }
    SCL(1);
    SCL(0);
    SDA(1);
} 

uint8_t I2C_Wait_Ack(void) // 等待从机应答
{
    uint8_t ack = 0;
    uint8_t flag = 10;
    I2C_SDA_IN();
    SDA(1);
    SCL(1);
    while((I2C_SDA_GET() == 1) && (flag)) // 等待从机应答
    {
        flag--;
    }
    if(flag <= 0)
    {
        I2C_Stop(); // 超时，产生停止信号
        return 1;
    }
    else
    {
        SCL(0);
        I2C_SDA_OUT();
    }
    return ack;
}

void I2C_Write_Byte(uint8_t data) // 主机向从机发送数据
{
    uint8_t i;
    I2C_SDA_OUT();
    SCL(0);
    for(i = 0; i < 8; i++)
    {
        SDA((data&0x80) >> 7); // 发送数据位
        SCL(1);
        SCL(0);
        data <<= 1; // 左移一位        
    }
}

uint8_t I2C_Read_Byte(void) // 从机读取数据
{
    uint8_t i, read_data = 0;
    I2C_SDA_IN();
    for(i = 0; i < 8; i++)
    {
        SCL(0);
        SCL(1);
        read_data <<= 1; // 左移一位
        if(I2C_SDA_GET())
        {
            read_data|=1; // 读取数据位
        }
    }
    SCL(0);
    return read_data;
}

// SHT20 读取湿温度
// 函数参数regaddr：寄存器地址
// 0xf3测量温度，0xf5测量湿度
uint16_t SHT20_Read(uint8_t regaddr)
{
    uint16_t data_h = 0;
    uint16_t data_l = 0;
    uint16_t temp = 0;
    I2C_Start();
    I2C_Write_Byte(0x80|0);
    if (I2C_Wait_Ack() == 1)
    {
        printf("error -1\r\n");

    }
    I2C_Write_Byte(regaddr);
    if (I2C_Wait_Ack() == 1)
    {
         printf("error -2\r\n");

    }
    do {
        I2C_Start();
        I2C_Write_Byte(0x80 | 1);
    } while (I2C_Wait_Ack() == 1);
    data_h = I2C_Read_Byte();
    I2C_Send_Ack(0);
    data_l = I2C_Read_Byte();
    I2C_Send_Ack(1);
    I2C_Stop();
    if (regaddr == 0xf3)
    {
        temp = ((data_h << 8)|data_h) / 65536.0 * 175.72 - 46.85;
    }
    if (regaddr == 0xf5)
    {
        temp = ((data_h << 8)|data_l) / 65536.0 * 125.0 - 6;
    }
    return temp;
}
