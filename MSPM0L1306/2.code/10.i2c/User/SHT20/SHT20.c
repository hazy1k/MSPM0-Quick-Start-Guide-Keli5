#include "SHT20.h"
#include "UART.h"
#include <stdio.h>

void I2C_GPIO_Init(void)
{
    DL_GPIO_initDigitalOutput(I2C_SCL_IOMUX);
    DL_GPIO_initDigitalOutput(I2C_SDA_IOMUX);
    DL_GPIO_setPins(GPIOA, I2C_SCL_PIN|I2C_SDA_PIN);
    DL_GPIO_enableOutput(GPIOA, I2C_SCL_PIN|I2C_SDA_PIN);
}

void IIC_Start(void)
{
    SDA_OUT();
        
    SCL(0);
    SDA(1);
    SCL(1);
    SDA(0);
    SCL(0);
}

void IIC_Stop(void)
{
    SDA_OUT();
        
    SCL(0);
    SDA(0);
        
    SCL(1);
    SDA(1);
}

void IIC_Send_Ack(uint8_t ack)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    if(!ack) SDA(0);
    else SDA(1);
    SCL(1);
    SCL(0);
    SDA(1);
}

uint8_t IIC_Wait_Ack(void)
{
    char ack = 0;
    unsigned char ack_flag = 10;
    SDA_IN();
    SDA(1);
    SCL(1);
    while((SDA_GET()==1) && (ack_flag))
    {
        ack_flag--;
    } 
    if(ack_flag <= 0)
    {
        IIC_Stop();
        return 1;
    }
    else
    {
        SCL(0);
        SDA_OUT();
    }
    return ack;
}

void IIC_Write(uint8_t data)
{
    int i = 0;
    SDA_OUT();
    SCL(0);
    for(i = 0; i < 8; i++)
    {
        SDA( (data & 0x80) >> 7);
        data<<=1;
        SCL(1);
        SCL(0);           
    }
}

uint8_t IIC_Read(void)
{
    unsigned char i,receive = 0;
    SDA_IN();
    for(i=0;i<8;i++ )
    {
        SCL(0);
        SCL(1);
        receive<<=1;
        if( SDA_GET() )
        {        
            receive|=1;   
        }
    }                
  return receive;
}

float SHT20_Read(uint8_t regaddr)
{        
    unsigned char data_H = 0;
    unsigned char data_L = 0;
    float temp = 0;
    IIC_Start();
    IIC_Write(0x80|0);
    if( IIC_Wait_Ack() == 1 ) printf("error -1\r\n");
    IIC_Write(regaddr);
    if( IIC_Wait_Ack() == 1 ) printf("error -2\r\n");       
    do{
    IIC_Start();
    IIC_Write(0x80|1);
    }while( IIC_Wait_Ack() == 1 );
    data_H = IIC_Read();
    IIC_Send_Ack(0);
    data_L = IIC_Read();
    IIC_Send_Ack(1);
    IIC_Stop();
    if( regaddr == 0xf3 )
    {
        temp = ((data_H<<8)|data_L) / 65536.0 * 175.72 - 46.85;
    }
    if( regaddr == 0xf5 )
    {
        temp = ((data_H<<8)|data_L) / 65536.0 * 125.0 - 6;
    }
   return temp;
}
