#include <ti_msp_dl_config.h>

/* 系统初始化 */
void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_SYSCTL_init();
}

/* GPIO 初始化 */
void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA); // 复位GPIOA
    DL_GPIO_enablePower(GPIOA); // 使能GPIOA
    delay_cycles(POWER_STARTUP_DELAY); // 电源启动延时
}

/* 系统时钟配置 */
void SYSCFG_DL_SYSCTL_init(void)
{
    // 低功耗模式配置为 SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0); // 设置BOR电压阈值 
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE); // 使用32MHz作为系统时钟
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE); // 关闭MCLK分频器
}
