/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      看门狗例程，看门狗产生中断
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2016-09-02
 * ---------------------------------------------------------------------------------------
 */


#include "driverlib.h"
#include "device.h"

uint32_t wakeCount;
uint32_t loopCount;

__interrupt void wakeupISR(void);

void main(void)
{
    //设备初始化
    Device_init();

    //中断初始化
    Interrupt_initModule();

    //中断向量初始化
    Interrupt_initVectorTable();

    //注册中断函数
    Interrupt_register(INT_WAKE, &wakeupISR);

	wakeCount = 0;
	loopCount = 0;

    //设定看门狗运行模式
    SysCtl_setWatchdogMode(SYSCTL_WD_MODE_INTERRUPT);

    //使能看门狗中断
    Interrupt_enable(INT_WAKE);

    EINT;
    ERTM;

    //复位看门狗计数器
    SysCtl_serviceWatchdog();

    //使能看门狗
    SysCtl_enableWatchdog();

    //循环
    for(;;)
    {
        loopCount++;

        // SysCtl_serviceWatchdog();
    }
}

//中断函数
__interrupt void
wakeupISR(void)
{
	wakeCount++;

    //相应中断
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

