## 例程16：超声波传感器US100数据采集

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

#### 4、学习US100传感器数据采集过程，利用串口中断解析数据

```
void UART_1_INST_IRQHandler(void)
{
  switch (DL_UART_Main_getPendingInterrupt(UART_1_INST))
  {
    case DL_UART_MAIN_IIDX_RX:
    {
      uint8_t ch = DL_UART_Main_receiveData(UART_1_INST);
			if(us100_prase_cnt==0) 
			{
				us100_prase_cnt=1;
				us100_buf[0]=ch;
			}
			else if(us100_prase_cnt==1) 
			{
				us100_buf[1]=ch;
				us100_distance=(256*(us100_buf[0])+(us100_buf[1]))/10.0f;//单位cm
				us100_prase_cnt=2;
			}
    }
    break;
    default:
      break;
  }
}
```



## MSPM0学习视频合集链接

##### [B站链接](https://www.bilibili.com/video/BV1Ei421Q7n9/)



## 例程所需的硬件资源
#### 1. MSPM0G3507核心板使用板载LED灯、RGB灯，其中LED对应PA0低电平点亮，RGB中R、G、B指示灯分别对应PB26、PB27、PB22。

#### 2.七线0.96寸 OLED显示屏，显示屏的时钟线SCL(D0）接核心板的PA17，显示屏的数据线SDA接核心板的PB15(D1），显示屏的复位线RES接核心板的PB16,显示屏的命令控制线DC接核心板的PB17,显示屏的片选线CS接核心板的PB20(可直接将显示屏插入核心板上LCD插槽测试)

#### 3.US100超声波传感器工作在串口模式（背面短接帽加上），US100的Trig/TX接核心板的PA8,，US100的Ecno/RX接核心板的PA9



## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 将7线OLED显示屏接入核心板
    b. 将us100超声波传感器接入核心板
    c. 编译并生成执行文件，点击Project->Rebuild all target files  
    d. 下载hex文件到核心板内，点击FLash->Download
    e. 按下核心板上复位按键运行
#### 2、实验现象

```
a. 实现现象，LED闪烁，亮灭时间间隔约为0.1S
b. oled显示屏会显示无名创新logo后，会显示计数器自加值，自加时间间隔约为0.1S
c. 显示屏上能显示US100超声波传感器的距离值，单位为cm
```

