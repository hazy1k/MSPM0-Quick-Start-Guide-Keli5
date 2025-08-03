#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X // MCU选择为MSPM0G3507
// 下面的配置是编译器兼容性设置，不必理会
#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

// TI官方的驱动库头文件
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

// C++外部链接声明
/*如果使用 C++ 编译器编译该文件，extern "C" 用于告诉编译器遵循 C 语言的链接规则，
这样 C++ 编译器不会对函数名进行 C++ 特有的名称修饰，从而确保 C 和 C++ 代码可以正常链接。*/
#ifdef __cplusplus
extern "C" {
#endif

// 宏定义电源启动延迟和MCU时钟频率
#define POWER_STARTUP_DELAY (16) // 启动电源后延迟16个时钟周期
#define CPUCLK_FREQ 32000000 // MCU时钟频率为32MHz

// 函数声明
void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_SYSCTL_init(void);

// C++外部链接结束
// 如果使用 C++ 编译器，则结束 extern "C" 块，确保函数按照 C 风格链接。
#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
