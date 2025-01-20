## 例程8：滴答定时器实现微秒、毫秒、延时、测量周期

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

#### 4、基于TI MSPM0标准库，学习滴答定时器相关库函数使用及微秒、毫秒、延时、测量周期的实现

```
volatile uint32_t sysTickUptime = 0;
void SysTick_Handler(void)
{
	 sysTickUptime++;
}


//返回 us
uint32_t micros(void)
{
	uint32_t systick_period=CPUCLK_FREQ/ 1000U;
	return sysTickUptime*1000+(1000*(systick_period-SysTick->VAL))/systick_period;
}


//返回 ms
uint32_t millis(void) {
	return micros() / 1000UL;
}

//延时us
void delayMicroseconds(uint32_t us) {
  uint32_t start = micros();
  while((int32_t)(micros() - start) < us) {
    // Do nothing
  };
}


void delay(uint32_t ms) {
  delayMicroseconds(ms * 1000UL);
}


void delay_ms(uint32_t x)
{
  delay(x);
}


void delay_us(uint32_t x)
{
  delayMicroseconds(x);
}

void Delay_Ms(uint32_t time)  //延时函数  
{   
	delay_ms(time);
}  

void Delay_Us(uint32_t time)  //延时函数  
{   
	delay_us(time);
}  


void get_systime(systime *sys)
{
  sys->last_time=sys->current_time;
  sys->current_time=micros()/1000.0f;//单位ms
  sys->period=sys->current_time-sys->last_time;
  sys->period_int=(uint16_t)(sys->period+0.5f);//四舍五入
}


float get_systime_ms(void)
{
  return millis();//单位ms
}

uint32_t get_systick_ms(void)
{
  return (uint32_t)(2*sysTickUptime);//单位ms
}

```

## MSPM0学习视频合集链接

##### [B站链接](https://www.bilibili.com/video/BV1Ei421Q7n9/)



## 例程所需的硬件资源
#### 1. MSPM0G3507核心板使用板载LED灯、RGB灯，其中LED对应PA0低电平点亮，RGB中R、G、B指示灯分别对应PB26、PB27、PB22。

#### 2.0.96寸 OLED显示屏，显示屏时钟线SCL接核心板的PA29，显示屏数据线SDA接核心板的PA30





## 例程软件工具（可通过QQ群文件下载，MSPM0 MCU学习技术交流QQ群号82874622）
#### 1. keil使用5.39版本
#### 2. 安装mspm0_sdk_2_00_01_00版本

#### 3.sysconfig图形化配置工具 sysconfig-1.20.0_3587-setup版本

#### 4.MSPM0 BSL烧录工具+uniflash编程工具



## 测试方法
#### 1、用keil打开本工程编译并下载程序运行
    a. 将oled显示屏接入核心板,SCL-->PA29，SDA-->PA30
    b. 编译并生成执行文件，点击Project->Rebuild all target files  
    c. 下载hex文件到核心板内，点击FLash->Download
    d. 按下核心板上复位按键运行
#### 2、实验现象

```
a. LED闪烁，周期约为350ms
b. RGB的红色灯闪烁，周期约为100ms
c. oled显示屏显示无名创新logo后，会显示当前运行时间，主函数while循环的运行周期，定时器的运行周期
```

