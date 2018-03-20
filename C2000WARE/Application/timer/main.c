/**
 * ---------------------------------------------------------------------------------------
 *          广州创龙电子科技有限公司
 *
 *          Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      定时器例程
 *
 * ---------------------------------------------------------------------------------------
 * 时间：2016-09-02
 * ---------------------------------------------------------------------------------------
 */

#include "driverlib.h"
#include "device.h"

void configCPUTimer(uint32_t cpuTimer, float freq, float period);
void InitCPUTimers(void);

uint16_t count;

__interrupt void cpuTimer0ISR(void);

void main(void)
{
    //系统初始化
	Device_init();
    Device_initGPIO();

    //配置GPIO口
    GPIO_setPadConfig(111, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(111, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(GPIO_111_GPIO111);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    //注册中断函数
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    //定时器参数配置
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000000);

    //使能计时器中断
    Interrupt_enable(INT_TIMER0);

    count = 0;

    EINT;
    ERTM;

    while(1);
}

void configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    count++;

    //初始化周期
    CPUTimer_setPeriod(cpuTimer, 0xFFFFFFFF);

    uint32_t temp;

    //公式
    temp = (uint32_t)(freq / 1000000 * period);

    //赋值
    CPUTimer_setPeriod(cpuTimer, temp);

    //停止
    CPUTimer_stopTimer(cpuTimer);
	
    // 用周期值重新加载所有计数器寄存器
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
       CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
	   
    //使能定时器外设模块的中断
    CPUTimer_enableInterrupt(cpuTimer);
    CPUTimer_startTimer(cpuTimer);
}

__interrupt void cpuTimer0ISR(void)
{
    GPIO_togglePin(111);

    //相应中断，清除位
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
