#include "LED.h"
#include "Timer.h"
#include <ti_msp_dl_config.h>

int main(void)
{
    SYSCFG_DL_init();
    LED_Init();
    Tim_Init();
    NVIC_ClearPendingIRQ(TIM_IRQN); // clear any pending interrupt
    NVIC_EnableIRQ(TIM_IRQN); // enable interrupt
    while(1)
    {
    }
}
