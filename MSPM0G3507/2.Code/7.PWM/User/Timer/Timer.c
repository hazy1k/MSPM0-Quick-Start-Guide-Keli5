#include "Timer.h"

// TIM时钟配置
static const DL_TimerG_ClockConfig gPWM_LEDClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK, // 时钟源选择为BUSCLK(4MHz)
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8, // 预分频系数为8
    .prescale = 0U, // 预分频值
}; // timerClkFreq = 4000000 Hz / (8 * (0 + 1)) = 4000000 Hz / 8 = 500000 Hz

// TIM工作参数配置
static const DL_TimerG_PWMConfig gPWM_LEDConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN, // PWM模式为边沿对齐
    .period = 1000, // 1000/500000 = 2ms
    .startTimer = DL_TIMER_START, // 启动定时器
};


void Tim_Init(void)
{
    /* GPIO初始化 */
    DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX, PWM_IOMUX_FUNC); // 设置PWM输出功能
    DL_GPIO_enableOutput(PWM_PORT, PWM_PIN); // 使能端口输出
    /* TIM初始化 */
    DL_TimerG_setClockConfig(TIM_PWM, (DL_TimerG_ClockConfig*)&gPWM_LEDClockConfig); // 设置时钟
    DL_TimerG_initPWMMode(TIM_PWM, (DL_TimerG_PWMConfig*)&gPWM_LEDConfig); // 设置PWM工作参数
    /* PWM初始化 */
    // 设置PWM的捕获比较值，此处设置为1000
    DL_TimerG_setCaptureCompareValue(TIM_PWM, 1000, DL_TIMER_CC_0_INDEX);
    // 1.设置初始输出低电平
    // 2.设置输出不反向
    // 3.设置输出源功能的选择
    // 4.设置指定的比较通道
    DL_TimerG_setCaptureCompareOutCtl(TIM_PWM, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
    DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    // 设置捕获比较更新方法为立即更新
    DL_TimerG_setCaptCompUpdateMethod(TIM_PWM, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
    DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_enableClock(TIM_PWM); // 使能定时器时钟
    DL_TimerG_setCCPDirection(TIM_PWM, DL_TIMER_CC0_OUTPUT); // 设置CC0输出方向为正脉冲
}



