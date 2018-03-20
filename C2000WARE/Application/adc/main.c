/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2017 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      ADC�����������
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2017-07-25
 * ---------------------------------------------------------------------------------------
 */
#include "driverlib.h"
#include "device.h"

//��������
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
    //��ʼ��
    Device_init();
    Device_initGPIO();

    //�жϳ�ʼ��
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //��ʼ��ADC
    initADCs();
	
    //��ʼ��ADCת���ź�
    initADCSOCs();

    EINT;
    ERTM;

    while(1)
    {
        //����ADC����
        startADC();
    }
}

void startADC(void)
{

	//ת�����ȴ���ɣ����洢���
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER1);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER1);

    // �ȴ�ADCA��ɣ�Ȼ��ȷ�ϱ�־
    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    // �ȴ�ADCB��ɣ�Ȼ��ȷ�ϱ�־
    while(ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    // �洢���
    adcAResult0 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    adcAResult1 = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    adcBResult0 = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    adcBResult1 = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);

    DEVICE_DELAY_US(1000);
}

void initADCs(void)
{
    //ʱ��Ԥ��Ƶ
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);

    //����ģʽ ����
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //����ת�������źŵ�ʱ��
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);

    //����ADC
    ADC_enableConverter(ADCA_BASE);
    ADC_enableConverter(ADCB_BASE);

    DEVICE_DELAY_US(1000);
}

void initADCSOCs(void)
{
    // ����ADCA��SOCs
	
    // - SOC0 ��ת�� pin A0.
    // - SOC1 ��ת�� pin A1.
    // -ȫ����ֻ�����������
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

	// - SOC0 ��ת�� pin B0.
    // - SOC1 ��ת�� pin B1.
    // -.ȫ����ֻ�����������
    // - For 12-bit resolution, a sampling window of 15 (75 ns at a 200MHz
    //   SYSCLK rate) will be used.  For 16-bit resolution, a sampling window
    //   of 64 (320 ns at a 200MHz SYSCLK rate) will be used.

    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                    ADC_CH_ADCIN0, 15);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY,
                    ADC_CH_ADCIN1, 15);

    //�ж�
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER1);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

}
