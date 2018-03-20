/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      ��ʱ������
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2016-09-02
 * ---------------------------------------------------------------------------------------
 */

#include "driverlib.h"
#include "device.h"

void configCPUTimer(uint32_t cpuTimer, float freq, float period);
void InitCPUTimers(void);

uint16_t count;

__interrupt void cpuTimer0ISR(void);

void main(void)
{
    //ϵͳ��ʼ��
	Device_init();
    Device_initGPIO();

    //����GPIO��
    GPIO_setPadConfig(111, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(111, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(GPIO_111_GPIO111);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    //ע���жϺ���
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    //��ʱ����������
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000000);

    //ʹ�ܼ�ʱ���ж�
    Interrupt_enable(INT_TIMER0);

    count = 0;

    EINT;
    ERTM;

    while(1);
}

void configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    count++;

    //��ʼ������
    CPUTimer_setPeriod(cpuTimer, 0xFFFFFFFF);

    uint32_t temp;

    //��ʽ
    temp = (uint32_t)(freq / 1000000 * period);

    //��ֵ
    CPUTimer_setPeriod(cpuTimer, temp);

    //ֹͣ
    CPUTimer_stopTimer(cpuTimer);
	
    // ������ֵ���¼������м������Ĵ���
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
       CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
	   
    //ʹ�ܶ�ʱ������ģ����ж�
    CPUTimer_enableInterrupt(cpuTimer);
    CPUTimer_startTimer(cpuTimer);
}

__interrupt void cpuTimer0ISR(void)
{
    GPIO_togglePin(111);

    //��Ӧ�жϣ����λ
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
