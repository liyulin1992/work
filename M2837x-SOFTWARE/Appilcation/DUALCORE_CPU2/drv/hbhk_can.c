/*
 * hbhk_can.c
 *
 *  Created on: 2017��2��28��
 *      Author: liyulin
 */
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"

unsigned char ucTXMsgData[4] = {1,2,3,4};
unsigned char ucRXMsgData[4];

__interrupt void canbISR(void);

//������ָ��
unsigned int volatile *p_c = (unsigned int *)0x9000;

//���������С
#define SIZE 8

//��������ID
#define RX_MSG_OBJ_ID 2

//�����־λ
unsigned int errorFlag;

//CAN��Ϣ������
tCANMsgObject sTXCANMessage;
tCANMsgObject sRXCANMessage;

void can_loop(void);
void can_init(void);

void can_init(void)
{
    //��ʼ��CAN������
    CANInit(CANA_BASE);

    // ѡ��CANģ���ʱ��Դ
    CANClkSourceSelect(CANA_BASE, 0);

    //����CAN�ı����ʣ�ʱ��
    CANBitRateSet(CANA_BASE, 200000000, 500000);

    DINT;

//    InitPieCtrl();

    //��ֹ����ж�
//    IER = 0x0000;
//    IFR = 0x0000;
//
//    InitPieVectTable();
    EALLOW;
    PieVectTable.CANA0_INT = canbISR;
    EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
    IER |= M_INT9;
    EINT;

    CANGlobalIntEnable(CANA_BASE, CAN_GLB_INT_CANINT0);

    //ʹ�ܲ���ģʽ�ͻػ�
    //ע�͵� �����ò���ģʽ
    EALLOW;

//    HWREG(CANA_BASE + CAN_O_CTL) |= CAN_CTL_CCE;
//    HWREG(CANA_BASE + CAN_O_TEST) = CAN_TEST_EXL;

    //���÷��ͽṹ��
    sTXCANMessage.ui32MsgID = 1;                      // CAN��ϢID
    sTXCANMessage.ui32MsgIDMask = 0;                  // ������
    sTXCANMessage.ui32Flags = 0;  // ʹ���ж�
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // ��Ϣ����
    sTXCANMessage.pucMsgData = ucTXMsgData;           // �������ݵ�ָ���ַ

    //���ý��ܽṹ��
    *(unsigned long *)ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = RX_MSG_OBJ_ID;          // CAN��ϢID
    sRXCANMessage.ui32MsgIDMask = 0;                  //������
    sRXCANMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   //��Ϣ����
    sRXCANMessage.pucMsgData = ucRXMsgData;           //�������ݵ�ָ���ַ

    //����message object���ڽ���
    CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);

    //ʹ��CAN
    CANEnable(CANA_BASE);

    EDIS;
}

void copy_can_to_ram(void)
{
    unsigned int j;

    for(j=0;j<SIZE;j++)
    {
        *p_c++ = ucRXMsgData[j];
    }
}

__interrupt void canbISR(void)
{
    uint32_t status;

    //��ȡ�ж�״̬λ
    status = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    // If the cause is a controller status interrupt, then get the status
    if(status == CAN_INT_INT0ID_STATUS)
    {

        //��ȡCAN��ͨѶ״̬��Ϣ
        status = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);

        // ����Ƿ����λ
        if(((status  & ~(CAN_ES_RXOK)) != 7) &&
           ((status  & ~(CAN_ES_RXOK)) != 0))
        {
            // �����־λ
            errorFlag = 1;
        }
    }

    //������IDλ
    else if(status == RX_MSG_OBJ_ID)
    {

        //������Ϣ
        CANMessageGet(CANA_BASE, RX_MSG_OBJ_ID, &sRXCANMessage, true);
        //���Ƶ��ڴ�
        copy_can_to_ram();
        CANIntClear(CANA_BASE, RX_MSG_OBJ_ID);
        errorFlag = 0;
    }

    //
    else
    {

    }

    //�����־λ
    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);

    //Ӧ��
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void can_loop(void)
{
    EALLOW;

    //����message object���ڷ���
    CANMessageSet(CANA_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);

    (*(unsigned long *)ucTXMsgData)++;

    EDIS;
}


