#include "LED.h"

void LED_Init(void) // LED��ʼ��
{
    DL_GPIO_initDigitalOutput(LED_IOMUX); // ��ʼ�������������
    DL_GPIO_clearPins(LED_PORT, LED_PIN); // ���PA14�ϵ�����
    DL_GPIO_enableOutput(LED_PORT, LED_PIN); // ʹ��PA14���
}
