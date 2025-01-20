## 例程15：利用GPIO外部中断和滴答定时器计时实现PPM信号的采集

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

#### 4、基于TI MSPM0标准库，学习外部中断相关的库函数使用以及PPM信号解析

```
void GROUP1_IRQHandler(void)
{
	switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
	{
		case USER_GPIO_INT_IIDX:
		{
			if(DL_GPIO_getEnabledInterruptStatus(USER_GPIO_PORT, USER_GPIO_PPM_PIN))
			{
				static uint32_t last_ppm_time=0,now_ppm_time=0;
				static uint8_t ppm_ready=0,ppm_sample_cnt=0;;
				static uint16_t ppm_time_delta=0;	

				last_ppm_time=now_ppm_time;//系统运行时间获取，单位us
				now_ppm_time=micros();//单位us	
				ppm_time_delta=now_ppm_time-last_ppm_time;//获取时间间隔
				//PPM解析开始
				if(ppm_ready==1)
				{
					if(ppm_time_delta>=2200)//帧结束电平至少2ms=2000us，由于部分老版本遥控器、
						//接收机输出PPM信号不标准，当出现解析异常时，尝试改小此值，该情况仅出现一例：使用天地飞老版本遥控器
					{
						//memcpy(PPM_Databuf,PPM_buf,ppm_sample_cnt*sizeof(uint16));
						ppm_ready = 1;
						ppm_sample_cnt=0;
						ppm_update_flag=1;
					}
					else if(ppm_time_delta>=900&&ppm_time_delta<=2100)
					{         
						PPM_buf[ppm_sample_cnt++]=ppm_time_delta;//对应通道写入缓冲区       
						if(ppm_sample_cnt>=8)//单次解析结束
						{
							memcpy(Receiver_PPM_Databuf,PPM_buf,ppm_sample_cnt*sizeof(uint16_t));
							//ppm_ready=0;
							ppm_sample_cnt=0;
						}
					}
					else  ppm_ready=0;
				}
				else if(ppm_time_delta>=2200)//帧结束电平至少2ms=2000us
				{
					ppm_ready=1;
					ppm_sample_cnt=0;
					ppm_update_flag=0;
				}
				
				DL_GPIO_clearInterruptStatus(USER_GPIO_PORT, USER_GPIO_PPM_PIN);
			}		
		
		}
		break;
	}
}
```



## MSPM0学习视频合集链接

##### [B站链接](https://www.bilibili.com/video/BV1Ei421Q7n9/)



## 例程所需的硬件资源
#### 1. MSPM0G3507核心板使用板载LED灯、RGB灯，其中LED对应PA0低电平点亮，RGB中R、G、B指示灯分别对应PB26、PB27、PB22。

#### 2.0.96寸 OLED显示屏，显示屏时钟线SCL接核心板的PA29，显示屏数据线SDA接核心板的PA30

#### 3.遥控器接收机的PPM信号输出接PA1



## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 将oled显示屏接入核心板,SCL-->PA29，SDA-->PA30
    b. PPM信号接PA1，注意需要和核心板共地
    b. 编译并生成执行文件，点击Project->Rebuild all target files  
    c. 下载hex文件到核心板内，点击FLash->Download
    d. 按下核心板上复位按键运行
#### 2、实验现象

```
a. LED闪烁，亮灭时间间隔约为0.1S
b. oled显示屏显示无名创新logo后，会显示运行时间、main函数主循环的周期，定时器中断测量时间等
c. 显示屏上能显示解析到的8个通道的ppm数据
```

