/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      DAC功能测试实验
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2017-07-25
 * ---------------------------------------------------------------------------------------
 */
 
#include <time.h>
#include <stdlib.h>
#include "driverlib.h"
#include "device.h"

uint16_t lowLimit = 410;
uint16_t highLimit = 3686;

void configureDAC(void);

void main(void)
{

    //初始化设备
    Device_init();

    //初始化PIE
    Interrupt_initModule();

    //初始化向量表
    Interrupt_initVectorTable();

    //产生随机数
    srand(time(NULL));

    //配置DAC
    configureDAC();

    //用随机数配置DAC输出电压
    while(1)
    {
        DAC_setShadowValue(DACA_BASE, ((rand() % (highLimit - lowLimit)) +
                                       lowLimit));
        DEVICE_DELAY_US(1);
    }
}

//配置DAC
void
configureDAC(void)
{
    //设定参考电压
    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_VDAC);

    //使能DAC输出
    DAC_enableOutput(DACA_BASE);

    //设定阴影值
    DAC_setShadowValue(DACA_BASE, 0);

    DEVICE_DELAY_US(10);
}

