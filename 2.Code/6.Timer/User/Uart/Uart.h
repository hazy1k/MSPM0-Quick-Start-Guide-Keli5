#ifndef __Uart_H
#define __Uart_H

#include "ti_msp_dl_config.h"

/* 中断配置 */
#define UART_INST            UART0
#define UART_IRQN   UART0_INT_IRQn
#define UARTx_IRQHandler UART0_IRQHandler

/* GPIO配置 */
#define UART_RX_PORT  GPIOA
#define UART_TX_PORT  GPIOA
#define UART_RX_PIN   DL_GPIO_PIN_11
#define UART_TX_PIN   DL_GPIO_PIN_10
#define UART_IOMUX_RX (IOMUX_PINCM22)
#define UART_IOMUX_TX (IOMUX_PINCM21)
#define UART_RX_FUNC  IOMUX_PINCM22_PF_UART0_RX
#define UART_TX_FUNC  IOMUX_PINCM21_PF_UART0_TX

/* 串口配置 */
#define UART_BAUD_RATE (9600)
#define UART_IBRD  (26)
#define UART_FBRD  (3)

void UART_0_init(void);
void uart0_send_char(char ch);
void uart0_send_string(char *str);

#endif 
