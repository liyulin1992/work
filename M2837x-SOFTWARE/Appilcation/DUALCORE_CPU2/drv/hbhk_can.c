/*
 * hbhk_can.c
 *
 *  Created on: 2017年2月28日
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

//复制用指针
unsigned int volatile *p_c = (unsigned int *)0x9000;

//接受数组大小
#define SIZE 8

//接受邮箱ID
#define RX_MSG_OBJ_ID 2

//错误标志位
unsigned int errorFlag;

//CAN消息控制器
tCANMsgObject sTXCANMessage;
tCANMsgObject sRXCANMessage;

void can_loop(void);
void can_init(void);

void can_init(void)
{
    //初始化CAN控制器
    CANInit(CANA_BASE);

    // 选择CAN模块的时钟源
    CANClkSourceSelect(CANA_BASE, 0);

    //配置CAN的比特率，时钟
    CANBitRateSet(CANA_BASE, 200000000, 500000);

    DINT;

//    InitPieCtrl();

    //禁止清除中断
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

    //使能测试模式和回环
    //注释掉 不适用测试模式
    EALLOW;

//    HWREG(CANA_BASE + CAN_O_CTL) |= CAN_CTL_CCE;
//    HWREG(CANA_BASE + CAN_O_TEST) = CAN_TEST_EXL;

    //配置发送结构体
    sTXCANMessage.ui32MsgID = 1;                      // CAN消息ID
    sTXCANMessage.ui32MsgIDMask = 0;                  // 无屏蔽
    sTXCANMessage.ui32Flags = 0;  // 使能中断
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // 消息长度
    sTXCANMessage.pucMsgData = ucTXMsgData;           // 发送内容的指针地址

    //配置接受结构体
    *(unsigned long *)ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = RX_MSG_OBJ_ID;          // CAN消息ID
    sRXCANMessage.ui32MsgIDMask = 0;                  //无屏蔽
    sRXCANMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   //消息长度
    sRXCANMessage.pucMsgData = ucRXMsgData;           //接受内容的指针地址

    //配置message object用于接受
    CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);

    //使能CAN
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

    //读取中断状态位
    status = CANIntStatus(CANA_BASE, CAN_INT_STS_CAUSE);

    // If the cause is a controller status interrupt, then get the status
    if(status == CAN_INT_INT0ID_STATUS)
    {

        //获取CAN的通讯状态信息
        status = CANStatusGet(CANA_BASE, CAN_STS_CONTROL);

        // 检查是否错误位
        if(((status  & ~(CAN_ES_RXOK)) != 7) &&
           ((status  & ~(CAN_ES_RXOK)) != 0))
        {
            // 错误标志位
            errorFlag = 1;
        }
    }

    //检查接受ID位
    else if(status == RX_MSG_OBJ_ID)
    {

        //接受信息
        CANMessageGet(CANA_BASE, RX_MSG_OBJ_ID, &sRXCANMessage, true);
        //复制到内存
        copy_can_to_ram();
        CANIntClear(CANA_BASE, RX_MSG_OBJ_ID);
        errorFlag = 0;
    }

    //
    else
    {

    }

    //清除标志位
    CANGlobalIntClear(CANA_BASE, CAN_GLB_INT_CANINT0);

    //应答
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

void can_loop(void)
{
    EALLOW;

    //配置message object用于发送
    CANMessageSet(CANA_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);

    (*(unsigned long *)ucTXMsgData)++;

    EDIS;
}


