## 例程14：利用GPIO外部中断实现正交编码脉冲采集

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

#### 4、基于TI MSPM0标准库，学习外部中断相关的库函数使用以及脉冲计数和鉴相原理

```
uint8_t x,y,z,sum;
void GROUP1_IRQHandler(void)
{
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) 
	{
		case USER_GPIO_INT_IIDX:
		if(DL_GPIO_getEnabledInterruptStatus(GPIOA, USER_GPIO_PULSE_A_PIN))
		{
			x=1;//A中断，则x=1;B中断，则x=0;
			
			//中断发生时，A相高电平，y=1；反之y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//中断发生时，B相高电平，z=1；反之z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;
			
			sum=x+y+z;//求和判断转动方向，偶数正转，奇数反转
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_A_PIN);
		}
		
		if(DL_GPIO_getEnabledInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN))
		{
			x=0;//A中断，则x=1;B中断，则x=0;
			//中断发生时，A相高电平，y=1；反之y=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_A_PIN)) y=1;
			else y=0;
			//中断发生时，B相高电平，z=1；反之z=0;
			if(DL_GPIO_readPins(GPIOA,USER_GPIO_PULSE_B_PIN)) z=1;
			else z=0;

			sum=x+y+z;//求和判断转动方向，偶数正转，奇数反转
			if(sum==0||sum==2) _encoder_l_count++;
			else _encoder_l_count--;
			
			DL_GPIO_clearInterruptStatus(GPIOA, USER_GPIO_PULSE_B_PIN);
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

#### 3.编码器A、B相脉冲分别接PA1、PA2



## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 将oled显示屏接入核心板,SCL-->PA29，SDA-->PA30
    b. 编码器A、B脉冲分别接PA1、PA2，注意编码器需要供电，并且需要和核心板共地
    b. 编译并生成执行文件，点击Project->Rebuild all target files  
    c. 下载hex文件到核心板内，点击FLash->Download
    d. 按下核心板上复位按键运行
#### 2、实验现象

```
a. LED闪烁，亮灭时间间隔约为0.1S
b. oled显示屏显示无名创新logo后，会显示计数器自加值，自加时间间隔约为0.1S
c. 手动转动电机时，显示屏能输出计数脉冲的累计值，某一方向的转动时，计数值的会增大，当反方向转动时计数值会减小
```

