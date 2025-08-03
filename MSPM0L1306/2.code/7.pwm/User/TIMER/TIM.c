#include "TIM.h"
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

/*
 * Timer clock configuration to be sourced by  / 8 (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   100000 Hz = 4000000 Hz / (8 * (39 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_LEDClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale = 39U
};

static const DL_TimerG_PWMConfig gPWM_LEDConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN, // 边沿对齐模式
    .period = 1000, // 重载值(999U)：1000ms * 100000Hz - 1 = 999U
    .startTimer = DL_TIMER_START, // 开始->启动
};

void PWM_LED_init(void) 
{
    /* GPIO初始化 */
    DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX, PWM_IOMUX_FUNC);
    DL_GPIO_enableOutput(PWM_PORT, PWM_PIN);
    /* 定时器初始化 */
    DL_TimerG_setClockConfig(PWM_TIM, (DL_TimerG_ClockConfig*)&gPWM_LEDClockConfig); // 时钟配置
    DL_TimerG_initPWMMode(PWM_TIM, (DL_TimerG_PWMConfig*)&gPWM_LEDConfig); // 工作模式配置
    DL_TimerG_setCaptureCompareValue(PWM_TIM, 1000, DL_TIMER_CC_0_INDEX); // 设置捕获比较值
    DL_TimerG_setCaptureCompareOutCtl(PWM_TIM, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX); // 设置捕获比较输出控制
    DL_TimerG_setCaptCompUpdateMethod(PWM_TIM, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_enableClock(PWM_TIM);
    DL_TimerG_setCCPDirection(PWM_TIM, DL_TIMER_CC0_OUTPUT);
}
