#include "ti_msp_dl_config.h"
#include "systick.h"
#include "Lcd_Driver.h"
#include "QDTFT_demo.h"




uint32_t cnt = 0;
int main(void)
{
  SYSCFG_DL_init();//芯片资源初始化,由SysConfig配置软件自动生成
	DL_TimerA_startCounter(TIMER_1_INST);
	NVIC_EnableIRQ(TIMER_1_INST_INT_IRQN);
	Lcd_Reset();
  while(1)
  {
		QDTFT_Test_Demo();
    //delay_ms(100);//延时0.1秒
  }
}


void TIMER_1_INST_IRQHandler(void)
{
  switch (DL_TimerG_getPendingInterrupt(TIMER_1_INST))
  {
    case DL_TIMERG_IIDX_ZERO:
    {
			DL_GPIO_togglePins(USER_GPIO_PORT, USER_GPIO_LED_PA0_PIN); //IO电平翻转
    }
    break;

    default:
      break;
  }
}