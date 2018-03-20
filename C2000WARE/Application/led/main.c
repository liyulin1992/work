/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      流水灯例程
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2016-09-02
 * ---------------------------------------------------------------------------------------
 */


#include "driverlib.h"
#include "device.h"
#include "stdio.h"

void GPIO_setup(void);

void main(void)
{
    //系统初始化
    Device_init();

    //初始化中断
    Interrupt_initModule();

    //初始化中断向量表
    Interrupt_initVectorTable();

    GPIO_setup();

    GPIO_writePin(111, 0);
    GPIO_writePin(112, 1);
    GPIO_writePin(113, 1);
    GPIO_writePin(114, 0);
    GPIO_writePin(4,1);
    GPIO_writePin(5,1);

    while(1)
    {
        GPIO_togglePin(111);
        DEVICE_DELAY_US(1000L * 500L);

        GPIO_togglePin(112);
        DEVICE_DELAY_US(1000L * 500L);

        GPIO_togglePin(113);
        DEVICE_DELAY_US(1000L * 500L);

        GPIO_togglePin(114);
        DEVICE_DELAY_US(1000L * 500L);
    }
}

void GPIO_setup(void)
{
    //gpio口类型、输出
    GPIO_setPadConfig(111, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(112, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(113, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(114, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(4,GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(5,GPIO_PIN_TYPE_STD);

    //方向
    GPIO_setDirectionMode(111, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(112, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(113, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(114, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_OUT);

    //GPIO复用
    GPIO_setPinConfig(GPIO_111_GPIO111);
    GPIO_setPinConfig(GPIO_112_GPIO112);
    GPIO_setPinConfig(GPIO_113_GPIO113);
    GPIO_setPinConfig(GPIO_114_GPIO114);
    GPIO_setPinConfig(GPIO_5_GPIO5);
    GPIO_setPinConfig(GPIO_4_GPIO4);
}
