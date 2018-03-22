/**
 * ---------------------------------------------------------------------------------------
 * 			���ݴ������ӿƼ����޹�˾
 *
 * 			Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *     CAN�ػ����ԣ��շ�����У�飬������ȷ���ӡ�������Ϣ�����͵������һλ���1����������λ
 *	   Ϊ2,3,4�����ܵ����ݿ����Ҽ�����ADD Watch�鿴�Ƿ��뷢��һ�¡�
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2016-09-02
 * ---------------------------------------------------------------------------------------
 */
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "pinmux.h"
#include "SciStdio.h"

#if 0
//*****************************************************************************
// A counter that keeps track of the number of times the transmit was
// successful.
//*****************************************************************************
volatile unsigned long g_ulMsgCount = 0;

//*****************************************************************************
// A flag to indicate that some transmission error occurred.
//*****************************************************************************
volatile unsigned long g_bErrFlag = 0;
#endif

//*****************************************************************************
// Configure the CAN and enter a loop to transmit periodic CAN messages.
//*****************************************************************************

unsigned char ucTXMsgData[4] = {1,2,3,4};
unsigned char ucRXMsgData[4];

int main(void)
{
	//CAN��Ϣ������
    tCANMsgObject sTXCANMessage;
    tCANMsgObject sRXCANMessage;

    //ϵͳ��ʼ��
	InitSysCtrl();

	//GPIO��ʼ��
	InitGpio();
//
//	SCIStdioInit();
//	SCIprintf("CAN LOOPBACK Test start...\r\n");

	//GPIO�������蹦�ܣ���һ������Ϊ���źã�������Ϊ���������
	//�������������ֲ�39ҳ��table4.4��TL28377�װ�������GPIO4��5
	//GPIO4����ΪCANTXA
	GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 6);

	//GPIO5����ΪCANRXA
	GPIO_SetupPinMux(5, GPIO_MUX_CPU1, 6);
//
//	GPIO_SetupPinOptions(5, GPIO_INPUT, GPIO_ASYNC);
//	GPIO_SetupPinOptions(4, GPIO_OUTPUT, GPIO_PUSHPULL);

	//��PINMUX�������ɵ�GPIO�������ú���
//	GPIO_setPinMuxConfig();

	//��ʼ��CAN������
    CANInit(CANA_BASE);

    // ѡ��CANģ���ʱ��Դ
    CANClkSourceSelect(CANA_BASE, 0);

    //����CAN�ı����ʣ�ʱ��
    CANBitRateSet(CANA_BASE, 200000000, 500000);

	DINT;

	InitPieCtrl();

    //��ֹ����ж�
	IER = 0x0000;
	IFR = 0x0000;

    InitPieVectTable();

    //ʹ�ܲ���ģʽ�ͻػ�
    //ע�͵� �����ò���ģʽ
    EALLOW;

    HWREG(CANA_BASE + CAN_O_CTL) |= CAN_CTL_CCE;
    HWREG(CANA_BASE + CAN_O_TEST) = CAN_TEST_EXL;

    sTXCANMessage.ui32MsgID = 1;                      // CAN��ϢID
    sTXCANMessage.ui32MsgIDMask = 0;                  // ������
    sTXCANMessage.ui32Flags = 0;  // ʹ���ж�
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // ��Ϣ����
    sTXCANMessage.pucMsgData = ucTXMsgData;           // �������ݵ�ָ���ַ

    // Initialize the message object that will be used for recieving CAN
    // messages.
    *(unsigned long *)ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = 1;                      // CAN��ϢID
    sRXCANMessage.ui32MsgIDMask = 0;                  //������
    sRXCANMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   //��Ϣ����
    sRXCANMessage.pucMsgData = ucRXMsgData;           //�������ݵ�ָ���ַ

    //����message object���ڽ���
    CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);

    //ʹ��CAN
    CANEnable(CANA_BASE);

    EDIS;

    for(;;)
    {
    	EALLOW;

    	//����message object���ڷ���
        CANMessageSet(CANA_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);

        // Now wait 1 second before continuing
        DELAY_US(1000*1000);

        //����
        CANMessageGet(CANA_BASE, 2, &sRXCANMessage, true);

        // Ensure the received data matches the transmitted data
//        if((*(unsigned long *)ucTXMsgData) != (*(unsigned long *)ucRXMsgData)){
//        	SCIprintf("Test failed!!!\r\n");
//        }

//        SCIprintf("CAN[1] = %d\n",ucRXMsgData[0]);

        (*(unsigned long *)ucTXMsgData)++;

        EDIS;
    }
}
