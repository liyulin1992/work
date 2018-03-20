/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      DMA内部传输
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2017-07-27
 * ---------------------------------------------------------------------------------------
 */

#include "driverlib.h"
#include "device.h"

//指定数据段
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

    //初始化中断
    Interrupt_initModule();

    //初始化PIE
    Interrupt_initVectorTable();

    //注册中断
    Interrupt_register(INT_DMA_CH6, &dmaCh6ISR);

    //初始化DMA
    initDMA();

    // DMA连接桥2
    SysCtl_selectSecMaster(0, SYSCTL_SEC_MASTER_DMA);

    for(i = 0; i < 128; i++)
    {
        sData[i] = i;
        rData[i] = 0;
    }

    //使能中断
    Interrupt_enable(INT_DMA_CH6);
    EINT;

    //启动DMA
    DMA_startChannel(DMA_CH6_BASE);
    flag = 0;

    // 等待中断完成
    while(!flag)
    {
       DMA_forceTrigger(DMA_CH6_BASE);

       DEVICE_DELAY_US(1000);
    }
}

//DMA初始化
void initDMA()
{
    //初始化DMA
    DMA_initController();
	
    const void *destAddr;
    const void *srcAddr;

    //源地址
    srcAddr = (const void *)sData;
	
    //目的地址
    destAddr = (const void *)rData;

    //配置地址
    DMA_configAddresses(DMA_CH6_BASE, destAddr, srcAddr);

    //brust配置
    DMA_configBurst(DMA_CH6_BASE,BURST,1,1);
    
	//传输配置
    DMA_configTransfer(DMA_CH6_BASE,TRANSFER,1,1);
    
	//触发模式
    DMA_configMode(DMA_CH6_BASE,DMA_TRIGGER_SOFTWARE, DMA_CFG_ONESHOT_DISABLE);

    //中断配置
    DMA_setInterruptMode(DMA_CH6_BASE,DMA_INT_AT_END);
    DMA_enableTrigger(DMA_CH6_BASE);
    DMA_enableInterrupt(DMA_CH6_BASE);
}

__interrupt void dmaCh6ISR(void)
{
    uint16_t i;

    DMA_stopChannel(DMA_CH6_BASE);

    //响应中断
    EALLOW;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);
    EDIS;

    for( i = 0; i < 128; i++ )
    {
        //校验
        if (rData[i] != i)
        {
            error++;
        }
    }

    // 完成标志
    flag = 1;
    return;
}
