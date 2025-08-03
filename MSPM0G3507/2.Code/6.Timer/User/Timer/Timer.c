#include "Timer.h"
#include "LED.h"
// TIM时钟配置
/* 具体计算过程
1. 首先确定时钟源，这里选择BUSCLK时钟（4MHz）
2. 确定分频系数，这里选择8分频，确定预分频值，这里选择99
3. 定时器频率 = 时钟源/(预分频值+1)分频系数 = 4000000/(99=1)8 = 40000Hz = 40KHz
*/
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK, // 时钟源选择，这里选择BUSCLK时钟
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8, // 时钟分频系数，这里选择8分频
    .prescale    = 99U, // 预分频值，这里选择99
};    

// TIM工作参数配置
/* 具体计算过程
1. 定时器的时钟频率已经配置为40KHz
2. 定时器的重载值 = (计数周期*定时器频率)-1 = 39999
3. 所以计数周期 = (3999+1)/ 40000 = 1s 
4. 即定时器每秒产生中断一次
*/
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIM_LOAD_VAL, // 定时器重载值，宏定义为39999U
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC, // 定时器工作模式，这里选择周期模式
    .startTimer = DL_TIMER_START, // 启动定时器，这里选择启动
};

// 初始化定时器
void Tim_Init(void)
{
    DL_TimerG_setClockConfig(TIMx, (DL_TimerG_ClockConfig*)&gTIMER_0ClockConfig); // 设置时钟配置
    DL_TimerG_initTimerMode(TIMx, (DL_TimerG_TimerConfig*)&gTIMER_0TimerConfig); // 初始化定时器工作参数
    DL_TimerG_enableInterrupt(TIMx, DL_TIMERG_INTERRUPT_ZERO_EVENT); // 使能定时器0中断，0溢出中断
    DL_TimerG_enableClock(TIMx); // 使能定时器
}

void TIM_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(TIMx)) 
    {
        case DL_TIMERG_IIDX_ZERO: 
            DL_GPIO_togglePins(LED_PORT, LED_PIN);
            break;
        default:
            break;
    }
}
