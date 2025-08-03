#include "TIM.h"
#include "LED.h"
/*
 * Timer clock configuration to be sourced by BUSCLK /  (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000 Hz = 4000000 Hz / (8 * (99 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK, // 时钟源->BUSCLK
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 99U, 
};// 定时器时钟频率被4000000Hz，除以8，再除以100，得到40000Hz
/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_0_INST_LOAD_VALUE = (1000 ms * 40000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_0TimerConfig = {
    .period     = TIMER_LOAD_VALUE, // 重载值(39999U)：40000Hz * 1000ms - 1 = 39999U
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC, // 定时器模式->周期模式
    .startTimer = DL_TIMER_START, // 开始->启动
};
void TIMER_Init(void) {
    DL_TimerG_setClockConfig(TIMERx,(DL_TimerG_ClockConfig *) &gTIMER_0ClockConfig);
    DL_TimerG_initTimerMode(TIMERx,(DL_TimerG_TimerConfig *) &gTIMER_0TimerConfig);
    DL_TimerG_enableInterrupt(TIMERx, DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMERx);
}

// 中断服务函数
void TIMERx_IRQHandler(void)
{
    switch(DL_TimerG_getPendingInterrupt(TIMERx))
    {
        case DL_TIMER_IIDX_ZERO: // 如果是0溢出中断
            DL_GPIO_togglePins(LED_PORT, LED_PIN);
            break;
        default:
            break;    
    }
}
