## 例程10：按键检测

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

#### 4、基于TI MSPM0标准库，学习GPIO电平读取相关库函数使用以及简单的按键防抖动检测原理

```
if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//按下为高电平
{
    delay_ms(10);
    if(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN))//延时之后再次检测
    {
    	while(DL_GPIO_readPins(KEY_PORT,KEY_S1_PIN));//等待按键释放
    	DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_R_PIN);//电平翻转驱动LED	
    }
}

if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//按下为低电平
{
    delay_ms(10);
    if(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN))//延时之后再次检测
    {
    	while(!DL_GPIO_readPins(PORTB_PORT,PORTB_S2_PIN));//等待按键释放
    	DL_GPIO_togglePins(PORTB_PORT,PORTB_RGB_G_PIN);//电平翻转驱动LED	
    }
}	
```

## MSPM0学习视频合集链接

##### [B站链接](https://www.bilibili.com/video/BV1Ei421Q7n9/)



## 例程所需的硬件资源
#### 1. MSPM0G3507核心板使用板载LED灯、RGB灯，其中LED对应PA0低电平点亮，RGB中R、G、B指示灯分别对应PB26、PB27、PB22。

 

## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 编译并生成执行文件，点击Project->Rebuild all target files  
    b. 下载hex文件到核心板内，点击FLash->Download
    c. 按下核心板上复位按键运行
#### 2、实验现象

```
a. LED闪烁，周期约为100ms
b. 按下板载按键BSL、K1可以控制RGB灯红色和绿色的点亮与熄灭
```

