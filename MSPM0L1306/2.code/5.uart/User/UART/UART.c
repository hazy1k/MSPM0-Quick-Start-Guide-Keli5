#include "UART.h"

volatile unsigned char uart_data = 0;

// 时钟配置
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_MFCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

// 工作参数配置
static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};
void SYSCFG_DL_UART_0_init(void)
{
    
    DL_GPIO_initPeripheralOutputFunction(GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);
    DL_UART_Main_setClockConfig(UART_0_INST, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);
    DL_UART_Main_init(UART_0_INST, (DL_UART_Main_Config *) &gUART_0Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 9600
     *  Actual baud rate: 9598.08
     */
    DL_UART_Main_setOversampling(UART_0_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_0_INST, UART_0_IBRD_4_MHZ_9600_BAUD, UART_0_FBRD_4_MHZ_9600_BAUD);

    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_RX);
    DL_UART_Main_enable(UART_0_INST);
}

// 串口发送单个字节
void uart0_send_char(char ch)
{
	while( DL_UART_isBusy(UART_0_INST) == true );
	DL_UART_Main_transmitData(UART_0_INST, ch);

}

// 串口发送字符串
void uart0_send_string(char* str)
{
	
	while(*str!=0&&str!=0)
	{
		uart0_send_char(*str++);
	}
}

// 兼容性相关
#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)
struct __FILE
{
    int handle;
};
#endif
FILE __stdout;
void _sys_exit(int x)
{
    x = x;
}
#endif

// 重定向printf
int fputc(int ch, FILE *stream)
{
	while( DL_UART_isBusy(UART_0_INST) == true);
	DL_UART_Main_transmitData(UART_0_INST, ch);
	return ch;
}

// 串口中断服务函数
void UART_0_INST_IRQHandler(void)
{
	switch( DL_UART_getPendingInterrupt(UART_0_INST) )
	{
		case DL_UART_IIDX_RX:
			uart_data = DL_UART_Main_receiveData(UART_0_INST);
			uart0_send_char(uart_data);
			break;
		
		default:
			break;
	}
}
