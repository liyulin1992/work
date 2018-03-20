/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      ���Ź����̣����Ź������ж�
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2016-09-02
 * ---------------------------------------------------------------------------------------
 */


#include "driverlib.h"
#include "device.h"

uint32_t wakeCount;
uint32_t loopCount;

__interrupt void wakeupISR(void);

void main(void)
{
    //�豸��ʼ��
    Device_init();

    //�жϳ�ʼ��
    Interrupt_initModule();

    //�ж�������ʼ��
    Interrupt_initVectorTable();

    //ע���жϺ���
    Interrupt_register(INT_WAKE, &wakeupISR);

	wakeCount = 0;
	loopCount = 0;

    //�趨���Ź�����ģʽ
    SysCtl_setWatchdogMode(SYSCTL_WD_MODE_INTERRUPT);

    //ʹ�ܿ��Ź��ж�
    Interrupt_enable(INT_WAKE);

    EINT;
    ERTM;

    //��λ���Ź�������
    SysCtl_serviceWatchdog();

    //ʹ�ܿ��Ź�
    SysCtl_enableWatchdog();

    //ѭ��
    for(;;)
    {
        loopCount++;

        // SysCtl_serviceWatchdog();
    }
}

//�жϺ���
__interrupt void
wakeupISR(void)
{
	wakeCount++;

    //��Ӧ�ж�
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

