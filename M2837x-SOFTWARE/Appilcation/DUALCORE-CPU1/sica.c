/**
 * ---------------------------------------------------------------------------------------
 * 			广州创龙电子科技有限公司
 *
 * 			Copyright 2015 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *     串口接受配置为中断模式，发送为查询模式。从PC端键入字符，DSP接受到后会发送回PC端。
 * ---------------------------------------------------------------------------------------
 * 时间：2016-12-17
 * ---------------------------------------------------------------------------------------
 */


#include "F28x_Project.h"

//接受中断函数
interrupt void sciaRxFifoIsr(void);

void scia_fifo_init(void);
void pie_setup(void);
void send_start_msg();

void scia_xmit(int a);
void scia_msg(char * msg);

char *msg;
Uint16 ReceivedChar;

void SCI_A()
{

	//设定PINMUX
	GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(28, GPIO_INPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 1);
	GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_ASYNC);

	DINT;

	//初始化PIE控制
	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	pie_setup();
	scia_fifo_init();

	//
	send_start_msg();
}

void send_start_msg()
{
	msg = "\r\n\n\nHello World!\0";
	scia_msg(msg);

	msg = "\r\nYou will enter a character, and the DSP will echo it back! \n\0";
	scia_msg(msg);
}

//初始化PIE
void pie_setup(void)
{
	//初始化向量表
	InitPieVectTable();

	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.SCIA_RX_INT = &sciaRxFifoIsr;
	EDIS;    // This is needed to disable write to EALLOW protected registers

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;   // PIE Group 9, INT1
	IER = 0x100;                         // Enable CPU INT9
	EINT;
}

interrupt void sciaRxFifoIsr(void)
{
    ReceivedChar = SciaRegs.SCIRXBUF.all;  // Read data

    msg = "  You sent: \n\0";
    scia_msg(msg);
    scia_xmit(ReceivedChar);

    SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x100;       // Issue PIE ack
}

//写发送寄存器
void scia_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all =a;
}

//传输字符
void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
}

void scia_fifo_init()
{
   SciaRegs.SCICCR.bit.STOPBITS = 0;      // 1 stop bit,  No loopback
   SciaRegs.SCICCR.bit.PARITYENA = 0;		// No parity,8 char bits,
   SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;            // async mode, idle-line protocol
   SciaRegs.SCICCR.bit.SCICHAR = 0x07;

   SciaRegs.SCICTL1.bit.TXENA = 1;     // enable TX, RX, internal SCICLK,
   SciaRegs.SCICTL1.bit.RXENA = 1;     // Disable RX ERR, SLEEP, TXWAKE

   SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

   //波特率115200
   SciaRegs.SCIHBAUD.all = 0x0000;
   SciaRegs.SCILBAUD.all = 0x0036;

   SciaRegs.SCIFFTX.all = 0xC022;
   SciaRegs.SCIFFRX.all = 0x0022;

   SciaRegs.SCIFFRX.bit.RXFFIENA = 1;

   SciaRegs.SCIFFCT.all=0x0;

   SciaRegs.SCICTL1.bit.SWRESET = 1;     // Relinquish SCI from Reset
   SciaRegs.SCIFFTX.bit.TXFIFORESET = 1;
   SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
}
