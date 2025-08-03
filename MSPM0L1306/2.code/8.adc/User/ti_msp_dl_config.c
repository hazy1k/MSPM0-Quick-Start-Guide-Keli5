#include "ti_msp_dl_config.h"

void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
}

void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_UART_Main_reset(UART0);
    DL_ADC12_reset(ADC0);
    DL_GPIO_enablePower(GPIOA);
    DL_UART_Main_enablePower(UART0);
    DL_ADC12_enablePower(ADC0);

    delay_cycles(POWER_STARTUP_DELAY);
}

void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_enableMFCLK();
}
