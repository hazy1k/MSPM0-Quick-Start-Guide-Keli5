#include <ti_msp_dl_config.h>

// 总系统初始化
void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower(); // 初始化电源管理
    SYSCFG_DL_SYSCTL_init(); // 初始化系统控制，配置时钟和电源管理
}

// 电源管理初始化
void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_reset(GPIOB); // 复位GPIOB

    DL_GPIO_enablePower(GPIOA); // 使能GPIOA的电源
    DL_GPIO_enablePower(GPIOB); // 使能GPIOB的电源

    delay_cycles(POWER_STARTUP_DELAY); // 延时一段时间，等待电源稳定
}

void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 设置系统时钟频率-32MHz
}
