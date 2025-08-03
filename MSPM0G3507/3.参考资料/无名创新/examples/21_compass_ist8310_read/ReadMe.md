## 例程21：IST8310磁力计传感器数据读取

## 例程目的
#### 1. 通过keil打开本工程、编译、并生成hex目标文件

```
keil需要5.39以上版本，并安装支持MSPM0L1306或者MSPM0G3507的芯片Pack包。  
本目录Keil工程依赖项：  
	a. TI SDK mspm0_sdk_2_00_01_00版本，并需要确保该SDK安装在C:\ti\目录下；  
	b. TI sysconfig_1.20.0，并确保安装在C:\ti\目录下；  
上述依赖项，若安装目录不同，则需要自行修改工程里面的头文件搜索路径和本开源工程根目录\tools\keil\syscfg.bat里面的搜索路径。  
```

#### 2. 学习通过SWD方式接好核心板与下载器，在下载调试器选择界面选择daplink进行下载

```
 daplink的swd连接目标核心板的swdio、daplink的clk连接目标核心板的swclk
```

#### 3. 学习使用uniflash编程工具，通过usb转ttl下载生成的hex文件到核心板
	直接通过usb线连接核心板上usb口，通过uniflash软件进行下载

#### 4、基于TI MSPM0标准库，学习SPI通讯协议相关的库函数使用

#### 5、学习使用标准库中硬件i2c函数的使用，对ist8310三轴磁场数据的读取

```
uint8_t Compass_Read_Data(int16_t *mag)//读取磁力计数据状态机
{
  static uint16_t compass_sampling_cnt=0;
  uint8_t buf[6];
  compass_sampling_cnt++;
  if(compass_sampling_cnt==1)
  {
		uint8_t val=0x01;
		I2C_WriteReg(IST8310_SLAVE_ADDRESS,IST8310_REG_CNTRL1,&val,1);//Single Measurement Mode
  }
  else if(compass_sampling_cnt>=20)//100ms
  {
		I2C_ReadReg(IST8310_SLAVE_ADDRESS,0x03,buf,6);
    mag[0]= (int16_t)((buf[1]<<8)|buf[0]);
    mag[1]= (int16_t)((buf[3]<<8)|buf[2]);
    mag[2]= (int16_t)((buf[5]<<8)|buf[4]);
	
    compass_sampling_cnt=0;
    return 1;
  }
  return 0;
}
```



## MSPM0学习视频合集链接

##### [B站链接](https://www.bilibili.com/video/BV1Ei421Q7n9/)



## 例程所需的硬件资源
#### 1. MSPM0G3507核心板使用板载LED灯、RGB灯，其中LED对应PA0低电平点亮，RGB中R、G、B指示灯分别对应PB26、PB27、PB22。

#### 2.七线0.96寸 OLED显示屏，显示屏的时钟线SCL(D0）接核心板的PA17，显示屏的数据线SDA接核心板的PB15(D1），显示屏的复位线RES接核心板的PB16,显示屏的命令控制线DC接核心板的PB17,显示屏的片选线CS接核心板的PB20(可直接将显示屏插入核心板上LCD插槽测试)

#### 3.ist8310的SCL接PA29，SDA接PA30



## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 将7线OLED显示屏接入核心板
    b. ist8310-scl-->PA29，ist8310-sda-->PA30
    b. 编译并生成执行文件，点击Project->Rebuild all target files  
    c. 下载hex文件到核心板内，点击FLash->Download
    d. 按下核心板上复位按键运行
#### 2、实验现象

```
a. 实现现象，LED闪烁，亮灭时间间隔约为0.01S
b. oled显示屏会显示无名创新logo后，会显示计数器自加值，自加时间间隔约为0.01S
c. oled显示屏上显示三轴磁场强度、偏航角度数据
```

