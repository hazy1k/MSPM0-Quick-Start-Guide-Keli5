#include "ti_msp_dl_config.h"

void SYSCFG_DL_init(void) // 系统初始化
{
    SYSCFG_DL_initPower(); // 电源初始化
    SYSCFG_DL_SYSCTL_init(); // 系统时钟初始化
}

void SYSCFG_DL_initPower(void) // 电源初始化
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_reset(GPIOB); // 复位GPIOB
    DL_UART_Main_reset(UART0); // 复位UART0
    DL_TimerG_reset(TIMG12); // 复位TimerG12

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB
    DL_UART_Main_enablePower(UART0); // 使能UART0
    DL_TimerG_enablePower(TIMG12); // 使能TimerG12

    delay_cycles(POWER_STARTUP_DELAY); // 等待启动时间
}

void SYSCFG_DL_SYSCTL_init(void) // 系统时钟初始化
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统频率为32MHz
	DL_SYSCTL_disableHFXT(); // 关闭HFXT
	DL_SYSCTL_disableSYSPLL(); // 关闭系统PLL
    DL_SYSCTL_enableMFCLK(); // 使能主时钟-MFCLK
}
/*
UART有三个时钟源：
1.BUSCLK:内部高速振荡器提供，出厂设置为32MHz
2.LFCLK:内部低速振荡器提供，低功耗时有效（32KHz）
3.MFCLK:固定的4MHz时钟
*/