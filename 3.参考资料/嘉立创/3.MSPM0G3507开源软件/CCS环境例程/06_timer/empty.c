#include "ti_msp_dl_config.h"

int main(void)
{
    SYSCFG_DL_init();
    //清除定时器中断标志
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    while (1)
    {

    }
}

//定时器的中断服务函数 已配置为1秒的周期
void TIMER_0_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            //将LED灯的状态翻转
            DL_GPIO_togglePins(LED_PORT, LED_PIN_14_PIN);
            break;

        default://其他的定时器中断
            break;
    }
}