#include "Uart.h"

volatile unsigned char uart_data = 0;

// UART时钟源配置
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_MFCLK, // 时钟源选择为MFCLK
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1 // 时钟分频系数为1
};

// UART工作参数配置
static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL, // 工作模式为正常模式
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX, // 传输方向为TX和RX
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE, // 流控模式为无流控
    .parity      = DL_UART_MAIN_PARITY_NONE, // 校验位为无校验
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS, // 数据位长度为8位
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE // 停止位为1位
};

// UART初始化
void UART_0_init(void)
{
    DL_GPIO_initPeripheralOutputFunction(UART_IOMUX_TX, UART_TX_FUNC); // 设置GPIO为输出(TX)
    DL_GPIO_initPeripheralInputFunction(UART_IOMUX_RX, UART_RX_FUNC); // 设置GPIO为输入(RX)
    DL_UART_Main_setClockConfig(UART_INST, (DL_UART_Main_ClockConfig*)&gUART_0ClockConfig); // 时钟源配置
    DL_UART_Main_init(UART_INST, (DL_UART_Main_Config*)&gUART_0Config); // UART工作参数配置
    DL_UART_Main_setOversampling(UART_INST, DL_UART_OVERSAMPLING_RATE_16X); // 采样率配置为16倍
    DL_UART_Main_setBaudRateDivisor(UART_INST, UART_IBRD, UART_FBRD); // 设置波特率分频值
    DL_UART_Main_enableInterrupt(UART_INST, DL_UART_MAIN_INTERRUPT_RX); // 使能接收中断
    DL_UART_Main_enable(UART_INST); // 使能UART
}

#if !defined(__MICROLIB)
// 如不使用微库，添加如下代码
#if (__ARMCLIB_VERSION <= 6000000)
// 如果编译是AC5，则使用如下代码
// struct __FILE
// {
//    int handle;
// };
#endif

FILE __stdout;  // 声明 __stdout

// 避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
#endif
// printf 重定向函数
int fputc(int ch, FILE *stream)
{
    while (DL_UART_isBusy(UART_INST) == true);  // 等待 UART 可用   
    DL_UART_Main_transmitData(UART_INST, ch);   // 发送数据
    return ch;
}
