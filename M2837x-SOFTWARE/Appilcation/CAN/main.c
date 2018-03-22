/**
 * ---------------------------------------------------------------------------------------
 * 			广州创龙电子科技有限公司
 *
 * 			Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *     CAN回环测试，收发进行校验，数据正确会打印出相关信息。发送的数组第一位会从1递增，其他位
 *	   为2,3,4。接受的数据可用右键——ADD Watch查看是否与发送一致。
 * ---------------------------------------------------------------------------------------
 * 时间：2016-09-02
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
	//CAN消息控制器
    tCANMsgObject sTXCANMessage;
    tCANMsgObject sRXCANMessage;

    //系统初始化
	InitSysCtrl();

	//GPIO初始化
	InitGpio();
//
//	SCIStdioInit();
//	SCIprintf("CAN LOOPBACK Test start...\r\n");

	//GPIO配置外设功能，第一个参数为引脚好，第三个为相关索引号
	//索引号在数据手册39页的table4.4，TL28377底板引脚是GPIO4、5
	//GPIO4配置为CANTXA
	GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 6);

	//GPIO5配置为CANRXA
	GPIO_SetupPinMux(5, GPIO_MUX_CPU1, 6);
//
//	GPIO_SetupPinOptions(5, GPIO_INPUT, GPIO_ASYNC);
//	GPIO_SetupPinOptions(4, GPIO_OUTPUT, GPIO_PUSHPULL);

	//用PINMUX工具生成的GPIO复用配置函数
//	GPIO_setPinMuxConfig();

	//初始化CAN控制器
    CANInit(CANA_BASE);

    // 选择CAN模块的时钟源
    CANClkSourceSelect(CANA_BASE, 0);

    //配置CAN的比特率，时钟
    CANBitRateSet(CANA_BASE, 200000000, 500000);

	DINT;

	InitPieCtrl();

    //禁止清除中断
	IER = 0x0000;
	IFR = 0x0000;

    InitPieVectTable();

    //使能测试模式和回环
    //注释掉 不适用测试模式
    EALLOW;

    HWREG(CANA_BASE + CAN_O_CTL) |= CAN_CTL_CCE;
    HWREG(CANA_BASE + CAN_O_TEST) = CAN_TEST_EXL;

    sTXCANMessage.ui32MsgID = 1;                      // CAN消息ID
    sTXCANMessage.ui32MsgIDMask = 0;                  // 无屏蔽
    sTXCANMessage.ui32Flags = 0;  // 使能中断
    sTXCANMessage.ui32MsgLen = sizeof(ucTXMsgData);   // 消息长度
    sTXCANMessage.pucMsgData = ucTXMsgData;           // 发送内容的指针地址

    // Initialize the message object that will be used for recieving CAN
    // messages.
    *(unsigned long *)ucRXMsgData = 0;
    sRXCANMessage.ui32MsgID = 1;                      // CAN消息ID
    sRXCANMessage.ui32MsgIDMask = 0;                  //无屏蔽
    sRXCANMessage.ui32Flags = MSG_OBJ_NO_FLAGS;
    sRXCANMessage.ui32MsgLen = sizeof(ucRXMsgData);   //消息长度
    sRXCANMessage.pucMsgData = ucRXMsgData;           //接受内容的指针地址

    //配置message object用于接受
    CANMessageSet(CANA_BASE, 2, &sRXCANMessage, MSG_OBJ_TYPE_RX);

    //使能CAN
    CANEnable(CANA_BASE);

    EDIS;

    for(;;)
    {
    	EALLOW;

    	//配置message object用于发送
        CANMessageSet(CANA_BASE, 1, &sTXCANMessage, MSG_OBJ_TYPE_TX);

        // Now wait 1 second before continuing
        DELAY_US(1000*1000);

        //接收
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
