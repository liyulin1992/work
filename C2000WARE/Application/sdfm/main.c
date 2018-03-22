/*
 * main.c
 *
 *  Created on: 2018年3月22日
 *      Author: lrk
 */
#include "driverlib.h"
#include "device.h"
#include <stdio.h>

#define MAX_SAMPLES               1024
#define SDFM_PIN_MUX_OPTION1      1
#define SDFM_PIN_MUX_OPTION2      2
#define SDFM_PIN_MUX_OPTION3      3

#define SDFM_FILTER_ENABLE 0x2U

uint16_t peripheralNumber;
uint16_t  filter1Result[MAX_SAMPLES];
uint16_t  filter2Result[MAX_SAMPLES];
uint16_t  filter3Result[MAX_SAMPLES];
uint16_t  filter4Result[MAX_SAMPLES];

#pragma DATA_SECTION(filter1Result, "Filter1_RegsFile");
#pragma DATA_SECTION(filter2Result, "Filter2_RegsFile");
#pragma DATA_SECTION(filter3Result, "Filter3_RegsFile");
#pragma DATA_SECTION(filter4Result, "Filter4_RegsFile");

void configureSDFMPins(uint16_t);
void configureSDFMMode(void);
void setPinConfig1(void);
void setPinConfig2(void);
void setPinConfig3(void);

__interrupt void sdfm1ISR(void);
__interrupt void sdfm2ISR(void);

uint16_t pinMuxOption;
 uint16_t hlt,llt;

void main(void)
{
    //系统初始化
    Device_init();

    Device_initGPIO();

    Interrupt_initVectorTable();

    //注册中断
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP5);
    Interrupt_register(INT_SD1, sdfm1ISR);
    Interrupt_register(INT_SD2, sdfm2ISR);

    //使能中断
    Interrupt_enable(INT_SD1);
    Interrupt_enable(INT_SD2);

    pinMuxOption = SDFM_PIN_MUX_OPTION1;

    // Configure GPIO pins as SDFM pins
    configureSDFMPins(pinMuxOption);

    configureSDFMMode();

    while(1);
}

void configureSDFMMode(void)
{
    //时钟
    SDFM_setupModulatorClock(SDFM1_BASE, SDFM_FILTER_1,
                           SDFM_MODULATOR_CLK_EQUAL_DATA_RATE);

    SDFM_setupModulatorClock(SDFM1_BASE, SDFM_FILTER_2,
                           SDFM_MODULATOR_CLK_EQUAL_DATA_RATE);

    SDFM_setupModulatorClock(SDFM1_BASE, SDFM_FILTER_3,
                           SDFM_MODULATOR_CLK_EQUAL_DATA_RATE);

    SDFM_setupModulatorClock(SDFM1_BASE, SDFM_FILTER_4,
                           SDFM_MODULATOR_CLK_EQUAL_DATA_RATE);

    //比较值设定
    hlt = 0x7FFF;
    llt = 0x0000;

    SDFM_configComparator(SDFM1_BASE,
         (SDFM_FILTER_1 | SDFM_FILTER_SINC_3 | SDFM_SET_OSR(32)),
         (SDFM_GET_LOW_THRESHOLD(llt) | SDFM_GET_HIGH_THRESHOLD(hlt)));
    SDFM_configComparator(SDFM1_BASE,
         (SDFM_FILTER_2 | SDFM_FILTER_SINC_3 | SDFM_SET_OSR(32)),
         (SDFM_GET_LOW_THRESHOLD(llt) | SDFM_GET_HIGH_THRESHOLD(hlt)));
    SDFM_configComparator(SDFM1_BASE,
         (SDFM_FILTER_3 | SDFM_FILTER_SINC_3 | SDFM_SET_OSR(32)),
         (SDFM_GET_LOW_THRESHOLD(llt) | SDFM_GET_HIGH_THRESHOLD(hlt)));
    SDFM_configComparator(SDFM1_BASE,
         (SDFM_FILTER_4 | SDFM_FILTER_SINC_3 | SDFM_SET_OSR(32)),
         (SDFM_GET_LOW_THRESHOLD(llt) | SDFM_GET_HIGH_THRESHOLD(hlt)));

    //滤波器设置
    SDFM_configDataFilter(SDFM1_BASE, (SDFM_FILTER_1 | SDFM_FILTER_SINC_3 |
              SDFM_SET_OSR(128)), (SDFM_DATA_FORMAT_16_BIT | SDFM_FILTER_ENABLE |
              SDFM_SHIFT_VALUE(0x0008)));

    SDFM_configDataFilter(SDFM1_BASE, (SDFM_FILTER_2 | SDFM_FILTER_SINC_3 |
              SDFM_SET_OSR(128)), (SDFM_DATA_FORMAT_16_BIT | SDFM_FILTER_ENABLE |
              SDFM_SHIFT_VALUE(0x0008)));

    SDFM_configDataFilter(SDFM1_BASE, (SDFM_FILTER_3 | SDFM_FILTER_SINC_3 |
              SDFM_SET_OSR(128)), (SDFM_DATA_FORMAT_16_BIT | SDFM_FILTER_ENABLE |
              SDFM_SHIFT_VALUE(0x0008)));

    SDFM_configDataFilter(SDFM1_BASE, (SDFM_FILTER_4 | SDFM_FILTER_SINC_3 |
              SDFM_SET_OSR(128)), (SDFM_DATA_FORMAT_16_BIT | SDFM_FILTER_ENABLE |
              SDFM_SHIFT_VALUE(0x0008)));

    //
    SDFM_enableMasterFilter(SDFM1_BASE);

    //禁止扩展功能
    SDFM_disableExternalReset(SDFM1_BASE, SDFM_FILTER_1);
    SDFM_disableExternalReset(SDFM1_BASE, SDFM_FILTER_2);
    SDFM_disableExternalReset(SDFM1_BASE, SDFM_FILTER_3);
    SDFM_disableExternalReset(SDFM1_BASE, SDFM_FILTER_4);

    //禁止使能相关中断
    SDFM_enableInterrupt(SDFM1_BASE, SDFM_FILTER_1,
                (SDFM_MODULATOR_FAILURE_INTERRUPT |
                 SDFM_DATA_FILTER_ACKNOWLEDGE_INTERRUPT));

    SDFM_enableInterrupt(SDFM1_BASE, SDFM_FILTER_2,
               (SDFM_MODULATOR_FAILURE_INTERRUPT |
                SDFM_DATA_FILTER_ACKNOWLEDGE_INTERRUPT));

    SDFM_enableInterrupt(SDFM1_BASE, SDFM_FILTER_3,
                (SDFM_MODULATOR_FAILURE_INTERRUPT |
                 SDFM_DATA_FILTER_ACKNOWLEDGE_INTERRUPT));

    SDFM_enableInterrupt(SDFM1_BASE, SDFM_FILTER_4,
               (SDFM_MODULATOR_FAILURE_INTERRUPT |
                SDFM_DATA_FILTER_ACKNOWLEDGE_INTERRUPT));

    SDFM_disableInterrupt(SDFM1_BASE, SDFM_FILTER_1,
                (SDFM_HIGH_LEVEL_THRESHOLD_INTERRUPT |
                 SDFM_LOW_LEVEL_THRESHOLD_INTERRUPT));

    SDFM_disableInterrupt(SDFM1_BASE, SDFM_FILTER_2,
               (SDFM_HIGH_LEVEL_THRESHOLD_INTERRUPT |
                SDFM_LOW_LEVEL_THRESHOLD_INTERRUPT));

    SDFM_disableInterrupt(SDFM1_BASE, SDFM_FILTER_3,
                (SDFM_HIGH_LEVEL_THRESHOLD_INTERRUPT |
                 SDFM_LOW_LEVEL_THRESHOLD_INTERRUPT));

    SDFM_disableInterrupt(SDFM1_BASE, SDFM_FILTER_4,
                (SDFM_HIGH_LEVEL_THRESHOLD_INTERRUPT |
                 SDFM_LOW_LEVEL_THRESHOLD_INTERRUPT));

    //使能MASTER中断
    SDFM_enableMasterInterrupt(SDFM1_BASE);

}
void configureSDFMPins(uint16_t sdfmPinOption)
{
    uint16_t pin;

    switch (sdfmPinOption)
    {
        case SDFM_PIN_MUX_OPTION1:
            for(pin = 16; pin <= 31; pin++)
            {
                GPIO_setDirectionMode(pin, GPIO_DIR_MODE_IN);
                GPIO_setMasterCore(pin, GPIO_CORE_CPU1);
                GPIO_setPadConfig(pin, GPIO_PIN_TYPE_STD);
                GPIO_setQualificationMode(pin, GPIO_QUAL_ASYNC);
            }
            setPinConfig1();
            break;

        case SDFM_PIN_MUX_OPTION2:
            for(pin = 48; pin <= 63; pin++)
            {
                GPIO_setDirectionMode(pin, GPIO_DIR_MODE_IN);
                GPIO_setMasterCore(pin, GPIO_CORE_CPU1);
                GPIO_setPadConfig(pin, GPIO_PIN_TYPE_STD);
                GPIO_setQualificationMode(pin, GPIO_QUAL_ASYNC);
            }
            setPinConfig2();
            break;

        case SDFM_PIN_MUX_OPTION3:
            for(pin = 122; pin <= 137; pin++)
            {
                GPIO_setMasterCore(pin, GPIO_CORE_CPU1);
                GPIO_setDirectionMode(pin, GPIO_DIR_MODE_IN);
                GPIO_setPadConfig(pin, GPIO_PIN_TYPE_STD);
                GPIO_setQualificationMode(pin, GPIO_QUAL_ASYNC);
            }
            setPinConfig3();
            break;
    }
}

//管脚复用配置
void setPinConfig1()
{
    GPIO_setPinConfig(GPIO_16_SD_D1);
    GPIO_setPinConfig(GPIO_17_SD_C1);
    GPIO_setPinConfig(GPIO_18_SD_D2);
    GPIO_setPinConfig(GPIO_19_SD_C2);
    GPIO_setPinConfig(GPIO_20_SD_D3);
    GPIO_setPinConfig(GPIO_21_SD_C3);
    GPIO_setPinConfig(GPIO_22_SD_D4);
    GPIO_setPinConfig(GPIO_23_SD_C4);
    GPIO_setPinConfig(GPIO_24_SD_D5);
    GPIO_setPinConfig(GPIO_25_SD_C5);
    GPIO_setPinConfig(GPIO_26_SD_D6);
    GPIO_setPinConfig(GPIO_27_SD_C6);
    GPIO_setPinConfig(GPIO_28_SD_D7);
    GPIO_setPinConfig(GPIO_29_SD_C7);
    GPIO_setPinConfig(GPIO_30_SD_D8);
    GPIO_setPinConfig(GPIO_31_SD_C8);
}

void setPinConfig2()
{
    GPIO_setPinConfig(GPIO_48_SD1_D1);
    GPIO_setPinConfig(GPIO_49_SD1_C1);
    GPIO_setPinConfig(GPIO_50_SD1_D2);
    GPIO_setPinConfig(GPIO_51_SD1_C2);
    GPIO_setPinConfig(GPIO_52_SD1_D3);
    GPIO_setPinConfig(GPIO_53_SD1_C3);
    GPIO_setPinConfig(GPIO_54_SD1_D4);
    GPIO_setPinConfig(GPIO_55_SD1_C4);
    GPIO_setPinConfig(GPIO_56_SD2_D1);
    GPIO_setPinConfig(GPIO_57_SD2_C1);
    GPIO_setPinConfig(GPIO_58_SD2_D2);
    GPIO_setPinConfig(GPIO_59_SD2_C2);
    GPIO_setPinConfig(GPIO_60_SD2_D3);
    GPIO_setPinConfig(GPIO_61_SD2_C3);
    GPIO_setPinConfig(GPIO_62_SD2_D4);
    GPIO_setPinConfig(GPIO_63_SD2_C4);
}

void setPinConfig3()
{
    GPIO_setPinConfig(GPIO_122_SD1_D1);
    GPIO_setPinConfig(GPIO_123_SD1_C1);
    GPIO_setPinConfig(GPIO_124_SD1_D2);
    GPIO_setPinConfig(GPIO_125_SD1_C2);
    GPIO_setPinConfig(GPIO_126_SD1_D3);
    GPIO_setPinConfig(GPIO_127_SD1_C3);
    GPIO_setPinConfig(GPIO_128_SD1_D4);
    GPIO_setPinConfig(GPIO_129_SD1_C4);
    GPIO_setPinConfig(GPIO_130_SD2_D1);
    GPIO_setPinConfig(GPIO_131_SD2_C1);
    GPIO_setPinConfig(GPIO_132_SD2_D2);
    GPIO_setPinConfig(GPIO_133_SD2_C2);
    GPIO_setPinConfig(GPIO_134_SD2_D3);
    GPIO_setPinConfig(GPIO_135_SD2_C3);
    GPIO_setPinConfig(GPIO_136_SD2_D4);
    GPIO_setPinConfig(GPIO_137_SD2_C4);
}

__interrupt void sdfm1ISR(void)
{
    uint32_t sdfmReadFlagRegister = 0;
    static uint16_t loopCounter1 = 0;

    SDFM_setOutputDataFormat(SDFM1_BASE, SDFM_FILTER_1,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM1_BASE, SDFM_FILTER_2,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM1_BASE, SDFM_FILTER_3,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM1_BASE, SDFM_FILTER_4,
                             SDFM_DATA_FORMAT_16_BIT);

    // Read SDFM flag register (SDIFLG)
    sdfmReadFlagRegister = HWREG(SDFM1_BASE + SDFM_O_SDIFLG);

    if(loopCounter1 <= MAX_SAMPLES)
    {
        // Read each SDFM filter output and store it in respective filter
        // result array
        filter1Result[loopCounter1] =
            SDFM_getFilterData(SDFM1_BASE, SDFM_FILTER_1);

        filter2Result[loopCounter1] =
            SDFM_getFilterData(SDFM1_BASE, SDFM_FILTER_2);

        filter3Result[loopCounter1] =
            SDFM_getFilterData(SDFM1_BASE, SDFM_FILTER_3);

        filter4Result[loopCounter1++] =
            SDFM_getFilterData(SDFM1_BASE, SDFM_FILTER_4);

        // Clear SDFM flag register (SDIFLG)
        SDFM_clearInterruptFlag(SDFM1_BASE, SDFM_MASTER_INTERRUPT_FLAG |
                                            0xFFFF);

        // Read SDFM flag register (SDIFLG)
        sdfmReadFlagRegister = HWREG(SDFM1_BASE + SDFM_O_SDIFLG);

        // If any flags have been set, stop
        if(sdfmReadFlagRegister != 0x0)
        {
            ESTOP0;
        }
    }
    else
    {
        ESTOP0;
    }

    // Acknowledge this __interrupt to receive more __interrupts from group 5
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP5);
}

// sdfm2ISR - SDFM 2 ISR
__interrupt void sdfm2ISR(void)
{
    uint32_t sdfmReadFlagRegister = 0;
    static uint16_t loopCounter1 = 0;

    SDFM_setOutputDataFormat(SDFM2_BASE, SDFM_FILTER_1,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM2_BASE, SDFM_FILTER_2,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM2_BASE, SDFM_FILTER_3,
                             SDFM_DATA_FORMAT_16_BIT);

    SDFM_setOutputDataFormat(SDFM2_BASE, SDFM_FILTER_4,
                             SDFM_DATA_FORMAT_16_BIT);

    // Read SDFM flag register (SDIFLG)
    sdfmReadFlagRegister = HWREG(SDFM2_BASE + SDFM_O_SDIFLG);

    if(loopCounter1 <= MAX_SAMPLES)
    {
        // Read each SDFM filter output and store it in respective filter
        // result array
        filter1Result[loopCounter1] =
            SDFM_getFilterData(SDFM2_BASE, SDFM_FILTER_1);

        filter2Result[loopCounter1] =
            SDFM_getFilterData(SDFM2_BASE, SDFM_FILTER_2);

        filter3Result[loopCounter1] =
            SDFM_getFilterData(SDFM2_BASE, SDFM_FILTER_3);

        filter4Result[loopCounter1++] =
            SDFM_getFilterData(SDFM2_BASE, SDFM_FILTER_4);

        // Clear SDFM flag register
        SDFM_clearInterruptFlag(SDFM2_BASE,
                                (SDFM_MASTER_INTERRUPT_FLAG | 0xFFFF));

        sdfmReadFlagRegister = HWREG(SDFM2_BASE + SDFM_O_SDIFLG);

        if(sdfmReadFlagRegister != 0x0)
        {
            ESTOP0;
        }
    }
    else
    {
        ESTOP0;
    }

    // Acknowledge this __interrupt to receive more __interrupts from group 5
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP5);
}
