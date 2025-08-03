#ifndef __LED_H
#define __LED_H

#include <ti_msp_dl_config.h>

/* LED信息宏定义 */
#define LED_PORT (GPIOA) // 选择GPIOA作为LED端口
#define LED_PIN (DL_GPIO_PIN_14) // 选择GPIOA14作为LED引脚
#define LED_IOMUX (IOMUX_PINCM15) // 复用索引选择

/* LED状态宏定义 */
#define LED_ON() DL_GPIO_setPins(LED_PORT, LED_PIN) // 高电平点亮
#define LED_OFF()  DL_GPIO_clearPins(LED_PORT, LED_PIN) // 低电平熄灭

/* 函数声明 */
void LED_Init(void);

#endif 
