#include "LED.h"

// LED��ʼ��
void LED_Init(void)
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // ��ʼ�������������
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // ���PA0�ϵ�����
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // ʹ��PA0���
}
