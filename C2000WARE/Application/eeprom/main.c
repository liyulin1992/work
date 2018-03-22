/*
 * main.c
 *
 *  Created on: 2018年3月19日
 *      Author: lrk
 */

#include "driverlib.h"
#include "device.h"

//器件地址
#define SLAVE_ADDRESS               0x50
#define EEPROM_HIGH_ADDR            0x00
#define EEPROM_LOW_ADDR             0x30
#define NUM_BYTES                   8
#define MAX_BUFFER_SIZE             14

//I2C消息传输状态
#define MSG_STATUS_INACTIVE         0x0000 // Message not in use, do not send
#define MSG_STATUS_SEND_WITHSTOP    0x0010 // Send message with stop bit
#define MSG_STATUS_WRITE_BUSY       0x0011 // Message sent, wait for stop
#define MSG_STATUS_SEND_NOSTOP      0x0020 // Send message without stop bit
#define MSG_STATUS_SEND_NOSTOP_BUSY 0x0021 // Message sent, wait for ARDY
#define MSG_STATUS_RESTART          0x0022 // Ready to become master-receiver
#define MSG_STATUS_READ_BUSY        0x0023 // Wait for stop before reading data

//错误信息
#define ERROR_BUS_BUSY              0x1000
#define ERROR_STOP_NOT_READY        0x5555
#define SUCCESS                     0x0000

struct I2CMsg
{
    uint16_t msgStatus;                  // Word stating what state msg is in.
                                         // See MSG_STATUS_* defines above.
    uint16_t slaveAddr;                  // Slave address tied to the message.
    uint16_t numBytes;                   // Number of valid bytes in message.
    uint16_t memoryHighAddr;             // EEPROM address of data associated
                                         // with message (high byte).
    uint16_t memoryLowAddr;              // EEPROM address of data associated
                                         // with message (low byte).
    uint16_t msgBuffer[MAX_BUFFER_SIZE]; // Array holding message data.
};

///////////////////////////////

//声明一个输出数据结构体
struct I2CMsg i2cMsgOut = {MSG_STATUS_SEND_WITHSTOP,
                           SLAVE_ADDRESS,
                           NUM_BYTES,
                           EEPROM_HIGH_ADDR,
                           EEPROM_LOW_ADDR,
                           0x01,                // Message bytes
                           0x23,
                           0x45,
                           0x67,
                           0x89,
                           0xAB,
                           0xCD,
                           0xEF};
//读取数据结构体
struct I2CMsg i2cMsgIn  = {MSG_STATUS_SEND_NOSTOP,
                           SLAVE_ADDRESS,
                           NUM_BYTES,
                           EEPROM_HIGH_ADDR,
                           EEPROM_LOW_ADDR};

// 中断用
struct I2CMsg *currentMsgPtr;

// 函数声明
void Init_I2C(void);
void I2c_Pinmux(void);

uint16_t ReadDate(struct I2CMsg *msg);
uint16_t WriteData(struct I2CMsg *msg);

uint16_t I2C_send(struct I2CMsg *msg);
uint16_t I2C_receive(struct I2CMsg *msg);

__interrupt void I2C_A_ISR(void);

void main(void)
{
    uint16_t i;

    //系统初始化
    Device_init();
    Device_initGPIO();

    //中断初始化
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //注册中断
    Interrupt_register(INT_I2CA, &I2C_A_ISR);

    Init_I2C();

    //初始化数组
    for (i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        i2cMsgIn.msgBuffer[i] = 0x0000;
    }

    currentMsgPtr = &i2cMsgOut;

    Interrupt_enable(INT_I2CA);
    EINT;
    ERTM;

    I2C_send(&i2cMsgOut);


    while(1)
    {

    }

}

void Init_I2C()
{
    // i2c管脚初始化
    GPIO_setPinConfig(GPIO_32_SDAA);
    GPIO_setPadConfig(32, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(32, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_33_SCLA);
    GPIO_setPadConfig(33, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(33, GPIO_QUAL_ASYNC);

    // i2c配置
    I2C_disableModule(I2CA_BASE);
    I2C_initMaster(I2CA_BASE, DEVICE_SYSCLK_FREQ, 400000, I2C_DUTYCYCLE_33);
    I2C_setBitCount(I2CA_BASE, I2C_BITCOUNT_8);
    I2C_setSlaveAddress(I2CA_BASE, SLAVE_ADDRESS);
    I2C_setEmulationMode(I2CA_BASE, I2C_EMULATION_FREE_RUN);

    //使能中断方式
    I2C_enableInterrupt(I2CA_BASE, I2C_INT_STOP_CONDITION |
                        I2C_INT_REG_ACCESS_RDY);

    // FIFO
    I2C_enableFIFO(I2CA_BASE);
    I2C_clearInterruptStatus(I2CA_BASE, I2C_INT_RXFF | I2C_INT_TXFF);

    I2C_enableModule(I2CA_BASE);
}



uint16_t WriteData(struct I2CMsg *msg)
{
    uint16_t i ;

    // 检测停止位
    if(I2C_getStopConditionStatus(I2CA_BASE))
    {
        return(ERROR_STOP_NOT_READY);
    }

    // 设置从地址
    I2C_setSlaveAddress(I2CA_BASE, SLAVE_ADDRESS);

    if(I2C_isBusBusy(I2CA_BASE))
    {
        return(ERROR_BUS_BUSY);
    }

    // 设置发送字节数
    I2C_setDataCount(I2CA_BASE, (msg->numBytes + 1));

    // 发送数据和地址
//    I2C_putData(I2CA_BASE, msg->memoryHighAddr);
    I2C_putData(I2CA_BASE, msg->memoryLowAddr);

    for (i = 0; i < msg->numBytes; i++)
    {
        I2C_putData(I2CA_BASE, msg->msgBuffer[i]);
    }

    // 发送起始位
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);

    return(SUCCESS);
}

uint16_t ReadDate(struct I2CMsg *msg)
{
    // 检测停止位
    if(I2C_getStopConditionStatus(I2CA_BASE))
    {
        return(ERROR_STOP_NOT_READY);
    }

    // 设置从地址
    I2C_setSlaveAddress(I2CA_BASE, SLAVE_ADDRESS);

    // 发送地址
    if(msg->msgStatus == MSG_STATUS_SEND_NOSTOP)
    {

        if(I2C_isBusBusy(I2CA_BASE))
        {
            return(ERROR_BUS_BUSY);
        }

        I2C_setDataCount(I2CA_BASE, 1);
//        I2C_putData(I2CA_BASE, msg->memoryHighAddr);
        I2C_putData(I2CA_BASE, msg->memoryLowAddr);
        I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
        I2C_sendStartCondition(I2CA_BASE);
    }

    else if(msg->msgStatus == MSG_STATUS_RESTART)
    {
        // 设置读取字节数以及重新启动接受
        I2C_setDataCount(I2CA_BASE, msg->numBytes);
        I2C_setConfig(I2CA_BASE, I2C_MASTER_RECEIVE_MODE);
        I2C_sendStartCondition(I2CA_BASE);
        I2C_sendStopCondition(I2CA_BASE);
    }

    return(SUCCESS);
}

uint16_t I2C_send(struct I2CMsg *msg)
{
//    struct I2CMsg *tempMsgPtr;
    uint16_t status;

    if(msg->msgStatus == MSG_STATUS_SEND_WITHSTOP)
    {
        status = WriteData(msg);

        if(status == SUCCESS)
        {
            msg->msgStatus = MSG_STATUS_WRITE_BUSY;
        }
    }

    return msg->msgStatus;
}

uint16_t I2C_receive(struct I2CMsg *msg)
{
    //查询发送状态
    if (i2cMsgOut.msgStatus == MSG_STATUS_INACTIVE)
    {
        //查询接受状态
        if(msg->msgStatus== MSG_STATUS_SEND_NOSTOP)
        {
            while(ReadDate(msg) != SUCCESS)
            {
                //等待应答
            }

            msg->msgStatus = MSG_STATUS_SEND_NOSTOP_BUSY;
        }

        else if(msg->msgStatus == MSG_STATUS_RESTART)
        {
            while(ReadDate(msg) != SUCCESS)
            {

            }

            msg->msgStatus = MSG_STATUS_READ_BUSY;
        }
    }

    return msg->msgStatus;
}

__interrupt void I2C_A_ISR(void)
{
    I2C_InterruptSource intSource;
    uint16_t i;

    //读取中断源
    intSource = I2C_getInterruptSource(I2CA_BASE);

    //
    if(intSource == I2C_INTSRC_STOP_CONDITION)
    {

        // If completed message was writing data, reset msg to inactive state
        if(currentMsgPtr->msgStatus == MSG_STATUS_WRITE_BUSY)
        {
            currentMsgPtr->msgStatus = MSG_STATUS_INACTIVE;
        }
        else
        {

            // If a message receives a NACK during the address setup portion of
            // the EEPROM read, the code further below included in the register
            // access ready interrupt source code will generate a stop
            // condition. After the stop condition is received (here), set the
            // message status to try again. User may want to limit the number
            // of retries before generating an error.
            if(currentMsgPtr->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
            {
                currentMsgPtr->msgStatus = MSG_STATUS_SEND_NOSTOP;
            }

            // If completed message was reading EEPROM data, reset message to
            // inactive state and read data from FIFO.
            else if(currentMsgPtr->msgStatus == MSG_STATUS_READ_BUSY)
            {
                currentMsgPtr->msgStatus = MSG_STATUS_INACTIVE;
                for(i=0; i < NUM_BYTES; i++)
                {
                    currentMsgPtr->msgBuffer[i] = I2C_getData(I2CA_BASE);
                }

                // Check received data
                for(i=0; i < NUM_BYTES; i++)
                {
                    if(i2cMsgIn.msgBuffer[i] == i2cMsgOut.msgBuffer[i])
                    {
                        //
                    }
                    else
                    {
                        //
                    }
                }
            }
        }
    }
    // Interrupt source = Register Access Ready
    //
    // This interrupt is used to determine when the EEPROM address setup
    // portion of the read data communication is complete. Since no stop bit
    // is commanded, this flag tells us when the message has been sent
    // instead of the SCD flag.
    else if(intSource == I2C_INTSRC_REG_ACCESS_RDY)
    {

        // If a NACK is received, clear the NACK bit and command a stop.
        // Otherwise, move on to the read data portion of the communication.
        if((I2C_getStatus(I2CA_BASE) & I2C_STS_NO_ACK) != 0)
        {
            I2C_sendStopCondition(I2CA_BASE);
            I2C_clearStatus(I2CA_BASE, I2C_STS_NO_ACK);
        }
        else if(currentMsgPtr->msgStatus == MSG_STATUS_SEND_NOSTOP_BUSY)
        {
            currentMsgPtr->msgStatus = MSG_STATUS_RESTART;
        }
    }
    else
    {

        // Generate some error from invalid interrupt source
        asm("   ESTOP0");
    }

    // Issue ACK to enable future group 8 interrupts
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
}
