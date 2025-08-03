#ifndef __UART_H__
#define __UART_H__

#include <ti_msp_dl_config.h>
#include <stdio.h>

/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_22
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_23
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM23)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM24)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM23_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM24_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_4_MHZ_9600_BAUD                                         (26)
#define UART_0_FBRD_4_MHZ_9600_BAUD                                          (3)

void DL_UART_0_init(void);
void uart0_send_char(char ch);
void uart0_send_string(char* str);

#endif
