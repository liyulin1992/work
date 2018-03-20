/*
 * mainc.c
 *
 *  Created on: 2018年3月14日
 *      Author: lrk
 */

#include "driverlib.h"
#include "device.h"

//
uint16_t loopCount;
uint16_t errorCount;

//函数声明
void initSCIALoopback(void);
void initSCIAFIFO(void);
void xmitSCIA(uint16_t a);
void error();

// Main
void main(void)
{
    uint16_t sendChar;
    uint16_t receivedChar;

    //系统初始化
    Device_init();

    //GPIO初始化
    Device_initGPIO();

    //配置SCI管脚
    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);

    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);

    //中断初始化
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //使能CPU中断
    Interrupt_enableMaster();

    //
    loopCount = 0;
    errorCount = 0;

    //初始化SCI
    initSCIAFIFO();
    initSCIALoopback();

    sendChar = 0;

    //回环
    for(;;)
    {
        xmitSCIA(sendChar);

        //等待状态寄存器
        while(SCI_getRxFIFOStatus(SCIA_BASE) == SCI_FIFO_RX0)
        {
            ;
        }

        //接受数据
        receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

        //校验
        if(receivedChar != sendChar)
        {
            error();
        }

        sendChar++;

        //
        sendChar &= 0x00FF;
        loopCount++;
    }
}


// error - Function to count errors
void error()
{
    errorCount++;
    asm("     ESTOP0");  // Uncomment to stop the test here
    for (;;);
}

//SCI配置
void initSCIALoopback()
{

    // 8 char bits, 1 stop bit, no parity. Baud rate is 9600.
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 9600, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_disableLoopback(SCIA_BASE);

    SCI_enableFIFO(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);

    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXRDY);
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);

    SCI_enableLoopback(SCIA_BASE);

    //使能
    SCI_enableModule(SCIA_BASE);
}

//
void xmitSCIA(uint16_t a)
{
    SCI_writeCharNonBlocking(SCIA_BASE, a);
}

//FIFO初始化
void initSCIAFIFO()
{
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);
    SCI_resetChannels(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}




