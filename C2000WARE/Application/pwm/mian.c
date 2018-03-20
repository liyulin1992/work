/**
 * ---------------------------------------------------------------------------------------
 *          ���ݴ������ӿƼ����޹�˾
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      PWM����
 *
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2016-09-02
 * ---------------------------------------------------------------------------------------
 */

#include "driverlib.h"
#include "device.h"

//���ò����궨��
#define EPWM1_TIMER_TBPRD  2000U
#define EPWM1_MAX_CMPA     1950U
#define EPWM1_MIN_CMPA       50U
#define EPWM1_MAX_CMPB     1950U
#define EPWM1_MIN_CMPB       50U

#define EPWM2_TIMER_TBPRD  2000U
#define EPWM2_MAX_CMPA     1950U
#define EPWM2_MIN_CMPA       50U
#define EPWM2_MAX_CMPB     1950U
#define EPWM2_MIN_CMPB       50U

#define EPWM3_TIMER_TBPRD  2000U
#define EPWM3_MAX_CMPA      950U
#define EPWM3_MIN_CMPA       50U
#define EPWM3_MAX_CMPB     1950U
#define EPWM3_MIN_CMPB     1050U

#define EPWM_CMP_UP           1U
#define EPWM_CMP_DOWN         0U

//�ṹ����PWM��Ϣ
typedef struct
{
    uint32_t epwmModule;
    uint16_t epwmCompADirection;
    uint16_t epwmCompBDirection;
    uint16_t epwmTimerIntCount;
    uint16_t epwmMaxCompA;
    uint16_t epwmMinCompA;
    uint16_t epwmMaxCompB;
    uint16_t epwmMinCompB;
}epwmInformation;

epwmInformation epwm1Info;;

//��������
void initEPWM1(void);

__interrupt void epwm1ISR(void);
void updateCompare(epwmInformation *epwmInfo);

// ������
void main(void)
{
    //��ʼ��
    Device_init();

    //GPIO��ʼ��
    Device_initGPIO();

    //��ʼ���ж�ģ��
    Interrupt_initModule();

    //ע���ж�������
    Interrupt_initVectorTable();

    //ע���жϼĴ���
    Interrupt_register(INT_EPWM1, &epwm1ISR);

    //����GPIO
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setPadConfig(1, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_1_EPWM1B);

    //��ֹʱ��
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    initEPWM1();

    //ʹ��ʱ��ͬ��
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //ʹ��PWM�ж�
    Interrupt_enable(INT_EPWM1);
    Interrupt_enable(INT_EPWM2);
    Interrupt_enable(INT_EPWM3);

    EINT;
    ERTM;

    for(;;)
    {
        NOP;
    }
}

//�жϷ�����
__interrupt void epwm1ISR(void)
{
    //���²���
    updateCompare(&epwm1Info);

    //�����־λ
    EPWM_clearEventTriggerInterruptFlag(EPWM1_BASE);

    //��Ӧ�ж�
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP3);
}

//PWM����
void initEPWM1()
{
    //����TBCLK
    EPWM_setTimeBasePeriod(EPWM1_BASE, EPWM1_TIMER_TBPRD);
    EPWM_setPhaseShift(EPWM1_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);

    //���ñȽϵ�Ԫ
    EPWM_setCounterCompareValue(EPWM1_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                EPWM1_MIN_CMPA);
    EPWM_setCounterCompareValue(EPWM1_BASE,
                                EPWM_COUNTER_COMPARE_B,
                                EPWM1_MAX_CMPB);

    //���ü���ģ��
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    EPWM_disablePhaseShiftLoad(EPWM1_BASE);
    EPWM_setClockPrescaler(EPWM1_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);

    //��Ӱģ��
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    //�趨����
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);

    //�ж���ص�����
    EPWM_setInterruptSource(EPWM1_BASE, EPWM_INT_TBCTR_ZERO);
    EPWM_enableInterrupt(EPWM1_BASE);
    EPWM_setInterruptEventCount(EPWM1_BASE, 3U);

    //
    epwm1Info.epwmCompADirection = EPWM_CMP_UP;
    epwm1Info.epwmCompBDirection = EPWM_CMP_DOWN;
    epwm1Info.epwmTimerIntCount = 0U;
    epwm1Info.epwmModule = EPWM1_BASE;
    epwm1Info.epwmMaxCompA = EPWM1_MAX_CMPA;
    epwm1Info.epwmMinCompA = EPWM1_MIN_CMPA;
    epwm1Info.epwmMaxCompB = EPWM1_MAX_CMPB;
    epwm1Info.epwmMinCompB = EPWM1_MIN_CMPB;
}



//�������ڸ���PWM����
void updateCompare(epwmInformation *epwmInfo)
{
    uint16_t compAValue;
    uint16_t compBValue;

    compAValue = EPWM_getCounterCompareValue(epwmInfo->epwmModule,
                                             EPWM_COUNTER_COMPARE_A);

    compBValue = EPWM_getCounterCompareValue(epwmInfo->epwmModule,
                                             EPWM_COUNTER_COMPARE_B);

    //ÿ10���ж�
    if(epwmInfo->epwmTimerIntCount == 10U)
    {
        epwmInfo->epwmTimerIntCount = 0U;


        if(epwmInfo->epwmCompADirection == EPWM_CMP_UP)
        {
            if(compAValue < (epwmInfo->epwmMaxCompA))
            {
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            ++compAValue);
            }
            else
            {
                epwmInfo->epwmCompADirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            --compAValue);
            }
        }

        else
        {
            if( compAValue == (epwmInfo->epwmMinCompA))
            {
                epwmInfo->epwmCompADirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            ++compAValue);
            }
            else
            {
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_A,
                                            --compAValue);
            }
        }


        if(epwmInfo->epwmCompBDirection == EPWM_CMP_UP)
        {
            if(compBValue < (epwmInfo->epwmMaxCompB))
            {
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_B,
                                            ++compBValue);
            }
            else
            {
                epwmInfo->epwmCompBDirection = EPWM_CMP_DOWN;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_B,
                                            --compBValue);
            }
        }

        else
        {
            if(compBValue == (epwmInfo->epwmMinCompB))
            {
                epwmInfo->epwmCompBDirection = EPWM_CMP_UP;
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_B,
                                            ++compBValue);
            }
            else
            {
                EPWM_setCounterCompareValue(epwmInfo->epwmModule,
                                            EPWM_COUNTER_COMPARE_B,
                                            --compBValue);
            }
        }
    }
    else
    {
        epwmInfo->epwmTimerIntCount++;
    }
}

