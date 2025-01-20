#include "ti_msp_dl_config.h"

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initDigitalOutput(PORTA_LED_IOMUX);

    DL_GPIO_initDigitalOutput(PORTB_RGB_R_IOMUX);

    DL_GPIO_initDigitalOutput(PORTB_RGB_G_IOMUX);

    DL_GPIO_initDigitalOutput(PORTB_RGB_B_IOMUX);

    DL_GPIO_clearPins(PORTA_PORT, PORTA_LED_PIN);
    DL_GPIO_enableOutput(PORTA_PORT, PORTA_LED_PIN);
    DL_GPIO_clearPins(PORTB_PORT, PORTB_RGB_R_PIN |
		PORTB_RGB_G_PIN |
		PORTB_RGB_B_PIN);
    DL_GPIO_enableOutput(PORTB_PORT, PORTB_RGB_R_PIN |
		PORTB_RGB_G_PIN |
		PORTB_RGB_B_PIN);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
}


