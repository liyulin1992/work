#include "driverlib.h"
#include "device.h"

#define PWM3_TIMER_MIN     500U
#define PWM3_TIMER_MAX     8000U
#define EPWM_TIMER_UP      1U
#define EPWM_TIMER_DOWN    0U

//全局变量
uint32_t ecap1IntCount;
uint32_t ecap1PassCount;
uint32_t epwm3TimerDirection;
volatile uint16_t cap2Count;
volatile uint16_t cap3Count;
volatile uint16_t cap4Count;
volatile uint16_t epwm3PeriodCount;

//函数声明
void error(void);
void initECAP(void);
void initEPWM(void);
__interrupt void ecap1ISR(void);


void main(void)
{
    //系统初始化
    Device_init();

    //GPIO初始化
    Device_initGPIO();

    //中断初始化
    Interrupt_initModule();
    Interrupt_initVectorTable();


    //配置PWM管脚
    GPIO_setPadConfig(4, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_4_EPWM3A);
    GPIO_setPadConfig(5, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_5_EPWM3B);

    //ECAP管脚
    XBAR_setInputPin(XBAR_INPUT7, 16);
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(16, GPIO_QUAL_ASYNC);

    //注册中断
    Interrupt_register(INT_ECAP1, &ecap1ISR);

    //设置ePWM and eCAP
    initEPWM();
    initECAP();

    //
    cap2Count = 0U;
    cap3Count = 0U;
    cap4Count = 0U;
    ecap1IntCount = 0U;
    ecap1PassCount = 0U;
    epwm3PeriodCount = 0U;

    //使能中断
    Interrupt_enable(INT_ECAP1);

    //使能全局中断
    EINT;
    ERTM;

    for(;;)
    {
       NOP;
    }
}

//配置PWM
void initEPWM()
{

    // Disable sync(Freeze clock to PWM as well)
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // PWM运行模式：
    // up-count mode
    // Action qualifier will toggle output on period match
    EPWM_setTimeBaseCounterMode(EPWM3_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(EPWM3_BASE, PWM3_TIMER_MIN);
    EPWM_setPhaseShift(EPWM3_BASE, 0U);
    EPWM_setActionQualifierAction(EPWM3_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_TOGGLE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setClockPrescaler(EPWM3_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_2);

    epwm3TimerDirection = EPWM_TIMER_UP;

    // Enable sync and clock to PWM
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

//配置ECAP
void initECAP()
{
    //清除中断
    ECAP_disableInterrupt(ECAP1_BASE,
                          (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                           ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                           ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                           ECAP_ISR_SOURCE_COUNTER_COMPARE));
    ECAP_clearInterrupt(ECAP1_BASE,
                        (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                         ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                         ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                         ECAP_ISR_SOURCE_COUNTER_COMPARE));

    //
    ECAP_disableTimeStampCapture(ECAP1_BASE);

    // 配置ECAP：
    //    Enable capture mode.
    //    One shot mode, stop capture at event 4.
    //    Set polarity of the events to rising, falling, rising, falling edge.
    //    time difference mode.
    //    Select input from XBAR7.
    ECAP_stopCounter(ECAP1_BASE);
    ECAP_enableCaptureMode(ECAP1_BASE);

    ECAP_setCaptureMode(ECAP1_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);

    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_4);

    XBAR_setInputPin(XBAR_INPUT7, 16);

    ECAP_enableLoadCounter(ECAP1_BASE);
    ECAP_setSyncOutMode(ECAP1_BASE, ECAP_SYNC_OUT_SYNCI);
    ECAP_startCounter(ECAP1_BASE);
    ECAP_enableTimeStampCapture(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);

    ECAP_enableInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
}

//中断服务函数
__interrupt void ecap1ISR(void)
{

    // Get the capture counts. Each capture should be 4x the ePWM count
    // because of the ePWM clock dividers.
    cap2Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);
    cap3Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_3);
    cap4Count = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_4);

    epwm3PeriodCount = EPWM_getTimeBasePeriod(EPWM3_BASE);

    if(cap2Count > ((epwm3PeriodCount * 4) + 4) ||
       cap2Count < ((epwm3PeriodCount * 4) - 4))
    {
        error();
    }

    if(cap3Count > ((epwm3PeriodCount * 4) + 4) ||
       cap3Count < ((epwm3PeriodCount * 4) - 4))
    {
        error();
    }

    if(cap4Count > ((epwm3PeriodCount * 4) + 4) ||
       cap4Count < ((epwm3PeriodCount * 4) - 4))
    {
        error();
    }

    ecap1IntCount++;

    //
    if(epwm3TimerDirection == EPWM_TIMER_UP)
    {
        if(epwm3PeriodCount < PWM3_TIMER_MAX)
        {
           EPWM_setTimeBasePeriod(EPWM3_BASE, ++epwm3PeriodCount);
        }
        else
        {
           epwm3TimerDirection = EPWM_TIMER_DOWN;
           EPWM_setTimeBasePeriod(EPWM3_BASE, ++epwm3PeriodCount);
        }
    }
    else
    {
        if(epwm3PeriodCount > PWM3_TIMER_MIN)
        {
            EPWM_setTimeBasePeriod(EPWM3_BASE, --epwm3PeriodCount);
        }
        else
        {
           epwm3TimerDirection = EPWM_TIMER_UP;
           EPWM_setTimeBasePeriod(EPWM3_BASE, ++epwm3PeriodCount);
        }
    }

    ecap1PassCount++;

    ECAP_clearInterrupt(ECAP1_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP1_BASE);

    //启动ECAP
    ECAP_reArm(ECAP1_BASE);

    //响应中断
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}

// error - Error function
void error()
{
    ESTOP0;
}
