#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X // 选择MSP430G350X系列
/* 以下代码是为了兼容性 */
#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

// TI官方库
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

/* 以下代码是为了兼容性 */
#ifdef __cplusplus
extern "C" {
#endif

#define POWER_STARTUP_DELAY (16) // 启动延时
#define CPUCLK_FREQ 32000000 // CPU时钟频率-32MHz

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_SYSCTL_init(void);

/* 兼容性代码 */
#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
