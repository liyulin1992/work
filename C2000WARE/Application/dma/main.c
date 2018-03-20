/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      DMA�ڲ�����
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2017-07-27
 * ---------------------------------------------------------------------------------------
 */

#include "driverlib.h"
#include "device.h"

//ָ�����ݶ�
#pragma DATA_SECTION(sData, "ramgs0");
#pragma DATA_SECTION(rData, "ramgs1");

#define BURST       8
#define TRANSFER    16

uint16_t error;
uint16_t sData[128];
uint16_t rData[128];
volatile uint16_t flag;

__interrupt void dmaCh6ISR(void);
void initDMA(void);

void main(void)
{
    uint16_t i;
    Device_init();

    //��ʼ���ж�
    Interrupt_initModule();

    //��ʼ��PIE
    Interrupt_initVectorTable();

    //ע���ж�
    Interrupt_register(INT_DMA_CH6, &dmaCh6ISR);

    //��ʼ��DMA
    initDMA();

    // DMA������2
    SysCtl_selectSecMaster(0, SYSCTL_SEC_MASTER_DMA);

    for(i = 0; i < 128; i++)
    {
        sData[i] = i;
        rData[i] = 0;
    }

    //ʹ���ж�
    Interrupt_enable(INT_DMA_CH6);
    EINT;

    //����DMA
    DMA_startChannel(DMA_CH6_BASE);
    flag = 0;

    // �ȴ��ж����
    while(!flag)
    {
       DMA_forceTrigger(DMA_CH6_BASE);

       DEVICE_DELAY_US(1000);
    }
}

//DMA��ʼ��
void initDMA()
{
    //��ʼ��DMA
    DMA_initController();
	
    const void *destAddr;
    const void *srcAddr;

    //Դ��ַ
    srcAddr = (const void *)sData;
	
    //Ŀ�ĵ�ַ
    destAddr = (const void *)rData;

    //���õ�ַ
    DMA_configAddresses(DMA_CH6_BASE, destAddr, srcAddr);

    //brust����
    DMA_configBurst(DMA_CH6_BASE,BURST,1,1);
    
	//��������
    DMA_configTransfer(DMA_CH6_BASE,TRANSFER,1,1);
    
	//����ģʽ
    DMA_configMode(DMA_CH6_BASE,DMA_TRIGGER_SOFTWARE, DMA_CFG_ONESHOT_DISABLE);

    //�ж�����
    DMA_setInterruptMode(DMA_CH6_BASE,DMA_INT_AT_END);
    DMA_enableTrigger(DMA_CH6_BASE);
    DMA_enableInterrupt(DMA_CH6_BASE);
}

__interrupt void dmaCh6ISR(void)
{
    uint16_t i;

    DMA_stopChannel(DMA_CH6_BASE);

    //��Ӧ�ж�
    EALLOW;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);
    EDIS;

    for( i = 0; i < 128; i++ )
    {
        //У��
        if (rData[i] != i)
        {
            error++;
        }
    }

    // ��ɱ�־
    flag = 1;
    return;
}
