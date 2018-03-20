/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      �����ж�
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2016-09-02
 * ---------------------------------------------------------------------------------------
 */
#include "driverlib.h"
#include "device.h"

//#define CPU_RATE 5.00L;
//#define DELAY (CPU_RATE/1000*6*510)

void GPIO_setup(void);
void Xint_setup(void);

unsigned int xint1count, xint2count,xint3count;

interrupt void key1_isr(void);
interrupt void key2_isr(void);
interrupt void key3_isr(void);

void main(void)
{
    //ϵͳ��ʼ��
    Device_init();

    //��ʼ���ж�
    Interrupt_initModule();

    //��ʼ���ж�������
    Interrupt_initVectorTable();

    GPIO_setup();

    Xint_setup();

    EINT;

    while(1);
}

void Xint_setup(void)
{
    Interrupt_register(INT_XINT1, &key1_isr);
    Interrupt_register(INT_XINT2, &key2_isr);
    Interrupt_register(INT_XINT3, &key3_isr);

    Interrupt_enable(INT_XINT1);
    Interrupt_enable(INT_XINT2);
    Interrupt_enable(INT_XINT3);

    GPIO_setInterruptPin(108, GPIO_INT_XINT1);
    GPIO_setInterruptPin(109,GPIO_INT_XINT2);
    GPIO_setInterruptPin(110,GPIO_INT_XINT3);

    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT3, GPIO_INT_TYPE_FALLING_EDGE);

    GPIO_enableInterrupt(GPIO_INT_XINT1);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
    GPIO_enableInterrupt(GPIO_INT_XINT3);
}

void GPIO_setup(void)
{
    //gpio�����͡����
    GPIO_setPadConfig(111, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(112, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(113, GPIO_PIN_TYPE_STD);

    //����
    GPIO_setDirectionMode(111, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(112, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(113, GPIO_DIR_MODE_OUT);

    //GPIO����
    GPIO_setPinConfig(GPIO_111_GPIO111);
    GPIO_setPinConfig(GPIO_112_GPIO112);
    GPIO_setPinConfig(GPIO_113_GPIO113);

    //����GPIO����ģʽ
    GPIO_setQualificationMode(108, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(109, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(110, GPIO_QUAL_6SAMPLE);

    GPIO_setDirectionMode(108, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(109, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(110, GPIO_DIR_MODE_IN);

    //Set qualification period for GPIO0 to GPIO7
    //Each sampling window is 510*SYSCLKOUT
    GPIO_setQualificationPeriod(108, 510);
    GPIO_setQualificationPeriod(109, 510);
    GPIO_setQualificationPeriod(110, 510);
}

interrupt void key1_isr(void)
{
    //��ת��ƽ
    GPIO_togglePin(111);
    xint1count++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void key2_isr(void)
{
    //��ת��ƽ
    GPIO_togglePin(112);
    xint2count++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

interrupt void key3_isr(void)
{
    //��ת��ƽ
    GPIO_togglePin(113);
    xint3count++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
}
