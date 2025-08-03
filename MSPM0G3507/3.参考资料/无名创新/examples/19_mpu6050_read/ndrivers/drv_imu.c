#include "ti_msp_dl_config.h"
#include "drv_i2c.h"
#include "drv_imu.h"

#define	SMPLRT_DIV		0x19
#define	MPU_CONFIG		0x1A
#define	GYRO_CONFIG		0x1B
#define	ACCEL_CONFIG	        0x1C
#define	ACCEL_XOUT_H	        0x3B
#define	ACCEL_XOUT_L	        0x3C
#define	ACCEL_YOUT_H	        0x3D
#define	ACCEL_YOUT_L	        0x3E
#define	ACCEL_ZOUT_H	        0x3F
#define	ACCEL_ZOUT_L	        0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B
#define	WHO_AM_I		0x75
#define USER_CTRL		0x6A
#define INT_PIN_CFG		0x37

void I2C_WriteReg(uint8_t DevAddr,uint8_t reg_addr, uint8_t *reg_data, uint8_t count);
void I2C_ReadReg(uint8_t DevAddr,uint8_t reg_addr, uint8_t *reg_data, uint8_t count);


void Single_WriteI2C(unsigned char SlaveAddress,unsigned char REG_Address,unsigned char REG_data)
{
	I2C_WriteReg(SlaveAddress,REG_Address,&REG_data,1);
}


unsigned char Single_ReadI2C(unsigned char SlaveAddress,unsigned char REG_Address)
{
	uint8_t data;
	I2C_ReadReg(SlaveAddress,REG_Address,&data,1);
	return data;
}

#define imu_adress 0x68

uint8_t read_imu[5];
void mpu6050_init(void)
{
  Single_WriteI2C(imu_adress,PWR_MGMT_1  , 0x00);//关闭所有中断,解除休眠
  Single_WriteI2C(imu_adress,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz
  Single_WriteI2C(imu_adress,MPU_CONFIG  , 0x02); //内部低通滤波频率，加速度计94hz,陀螺仪98hz
  Single_WriteI2C(imu_adress,GYRO_CONFIG , 0x08);//500deg/s
  Single_WriteI2C(imu_adress,ACCEL_CONFIG, 0x10);// Accel scale 8g (4096 LSB/g)
	
	read_imu[0]=Single_ReadI2C(imu_adress,PWR_MGMT_1);
	read_imu[1]=Single_ReadI2C(imu_adress,SMPLRT_DIV);
	read_imu[2]=Single_ReadI2C(imu_adress,MPU_CONFIG);
	read_imu[3]=Single_ReadI2C(imu_adress,GYRO_CONFIG);
	read_imu[4]=Single_ReadI2C(imu_adress,ACCEL_CONFIG);
}


void mpu6050_read(int16_t *gyro,int16_t *accel,float *temperature)
{
	uint8_t buf[14];
	int16_t temp;
	I2C_ReadReg(imu_adress,ACCEL_XOUT_H,buf,14);
	accel[0]=(int16_t)((buf[0]<<8)|buf[1]);
	accel[1]=(int16_t)((buf[2]<<8)|buf[3]);
	accel[2]=(int16_t)((buf[4]<<8)|buf[5]);	
	temp		=(int16_t)((buf[6]<<8)|buf[7]);
	gyro[0]	=(int16_t)((buf[8]<<8)|buf[9]);
	gyro[1]	=(int16_t)((buf[10]<<8)|buf[11]);
	gyro[2]	=(int16_t)((buf[12]<<8)|buf[13]);	
	*temperature=36.53f+(float)(temp/340.0f);	
}


