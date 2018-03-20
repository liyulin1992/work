/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      DAC���ܲ���ʵ��
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2017-07-25
 * ---------------------------------------------------------------------------------------
 */
 
#include <time.h>
#include <stdlib.h>
#include "driverlib.h"
#include "device.h"

uint16_t lowLimit = 410;
uint16_t highLimit = 3686;

void configureDAC(void);

void main(void)
{

    //��ʼ���豸
    Device_init();

    //��ʼ��PIE
    Interrupt_initModule();

    //��ʼ��������
    Interrupt_initVectorTable();

    //���������
    srand(time(NULL));

    //����DAC
    configureDAC();

    //�����������DAC�����ѹ
    while(1)
    {
        DAC_setShadowValue(DACA_BASE, ((rand() % (highLimit - lowLimit)) +
                                       lowLimit));
        DEVICE_DELAY_US(1);
    }
}

//����DAC
void
configureDAC(void)
{
    //�趨�ο���ѹ
    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_VDAC);

    //ʹ��DAC���
    DAC_enableOutput(DACA_BASE);

    //�趨��Ӱֵ
    DAC_setShadowValue(DACA_BASE, 0);

    DEVICE_DELAY_US(10);
}

