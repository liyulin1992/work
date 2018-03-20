/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      ADC软件触发测试
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2017-07-25
 * ---------------------------------------------------------------------------------------
 */
#include "driverlib.h"
#include "device.h"

//存结果变量
uint16_t adcAResult0;
uint16_t adcAResult1;
uint16_t adcBResult0;
uint16_t adcBResult1;

void initADCs(void);
void initADCSOCs(void);
void startADC(void);

unsigned int data_t[4] = {1,5,3,4};

unsigned int p_t;
unsigned int p_s;

const unsigned int *p = data_t;

void main(void)
{
    //初始化
    Device_init();
    Device_initGPIO();

    //中断初始化
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //初始化ADC
    initADCs();
	
    //初始化ADC转换信号
    initADCSOCs();

    EINT;
    ERTM;

    while(1)
    {
        //启动ADC任务
        startADC();
    }
}

void startADC(void)
{

	//转换，等待完成，并存储结果
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER1);

    // 等待ADCA完成，然后确认标志
    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    // 等待ADCB完成，然后确认标志
    while(ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    // 存储结果
    adcAResult0 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcAResult1 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    adcBResult0 = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    adcBResult1 = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);

    DEVICE_DELAY_US(1000);
}

void initADCs(void)
{
    //时钟预分频
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);

    //工作模式 单端
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //设置转换结束信号的时序
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);

    //启动ADC
    ADC_enableConverter(ADCA_BASE);
    ADC_enableConverter(ADCB_BASE);

    DEVICE_DELAY_US(1000);
}

void initADCSOCs(void)
{
    // 配置ADCA的SOCs
	
    // - SOC0 将转换 pin A0.
    // - SOC1 将转换 pin A1.
    // -全部都只将被软件触发
    // - For 12-bit resolution, a sampling window of 15 (75 ns at a 200MHz
    //   SYSCLK rate) will be used.  For 16-bit resolution, a sampling window
    //   of 64 (320 ns at a 200MHz SYSCLK rate) will be used.

    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                     ADC_CH_ADCIN0, 15);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY,
                     ADC_CH_ADCIN1, 15);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

	// - SOC0 将转换 pin B0.
    // - SOC1 将转换 pin B1.
    // -.全部都只将被软件触发
    // - For 12-bit resolution, a sampling window of 15 (75 ns at a 200MHz
    //   SYSCLK rate) will be used.  For 16-bit resolution, a sampling window
    //   of 64 (320 ns at a 200MHz SYSCLK rate) will be used.

    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                    ADC_CH_ADCIN0, 15);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY,
                    ADC_CH_ADCIN1, 15);

    //中断
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

}
