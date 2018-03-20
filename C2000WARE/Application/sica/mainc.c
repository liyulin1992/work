/*
 * mainc.c
 *
 *  Created on: 2018��3��14��
 *      Author: lrk
 */

#include "driverlib.h"
#include "device.h"

//
uint16_t loopCount;
uint16_t errorCount;

//��������
void initSCIALoopback(void);
void initSCIAFIFO(void);
void xmitSCIA(uint16_t a);
void error();

// Main
void main(void)
{
    uint16_t sendChar;
    uint16_t receivedChar;

    //ϵͳ��ʼ��
    Device_init();

    //GPIO��ʼ��
    Device_initGPIO();

    //����SCI�ܽ�
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

    //�жϳ�ʼ��
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //ʹ��CPU�ж�
    Interrupt_enableMaster();

    //
    loopCount = 0;
    errorCount = 0;

    //��ʼ��SCI
    initSCIAFIFO();
    initSCIALoopback();

    sendChar = 0;

    //�ػ�
    for(;;)
    {
        xmitSCIA(sendChar);

        //�ȴ�״̬�Ĵ���
        while(SCI_getRxFIFOStatus(SCIA_BASE) == SCI_FIFO_RX0)
        {
            ;
        }

        //��������
        receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

        //У��
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

//SCI����
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

    //ʹ��
    SCI_enableModule(SCIA_BASE);
}

//
void xmitSCIA(uint16_t a)
{
    SCI_writeCharNonBlocking(SCIA_BASE, a);
}

//FIFO��ʼ��
void initSCIAFIFO()
{
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);
    SCI_resetChannels(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}




