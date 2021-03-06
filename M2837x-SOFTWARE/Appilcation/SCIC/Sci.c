// TI File $Revision: /main/2 $
// Checkin $Date: April 29, 2005   11:08:40 $
//###########################################################################
//
// FILE:	DSP281x_Sci.c
//
// TITLE:	DSP281x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: DSP281x C/C++ Header Files V1.20 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File


//---------------------------------------------------------------------------
// InitSPI: 
//---------------------------------------------------------------------------
// This function initializes the SPI(s) to a known state.
//
void InitScicGpio(void)
{
	EALLOW;
	GpioCtrlRegs.GPEMUX1.bit.GPIO139 = 1;
	GpioCtrlRegs.GPEMUX1.bit.GPIO140 = 1;
	EDIS;
}

// Initalize the SCI FIFO
void Scic_fifo_init()
{
    ScicRegs.SCIFFTX.all=0xE040;
    ScicRegs.SCIFFRX.all=0x2044;
    ScicRegs.SCIFFCT.all=0x0;
}

// Test 1,Scic  DLB, 8-bit word, baud rate 0x000F, default, 1 STOP bit, no parity
void Scic_echoback_init()
{
    // Note: Clocks were turned on to the Scic peripheral
    // in the InitSysCtrl() function

 	ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
	ScicRegs.SCICTL2.all =0x0003;
	ScicRegs.SCICTL2.bit.TXINTENA =1;
	ScicRegs.SCICTL2.bit.RXBKINTENA =1;

    //
    // Scic at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
	// 115200 baud
	ScicRegs.SCIHBAUD.all    =0x0000;
    ScicRegs.SCILBAUD.all    =0x0036;

	ScicRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
}


void SCIConsolePutc(unsigned char data)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF.all = data;
}

unsigned char SCIConsoleGetc(void)
{
	unsigned char ReceivedChar;

    // Wait for inc character
    while(ScicRegs.SCIFFRX.bit.RXFFST!=1) { } // wait for XRDY =1 for empty state

    // Get character
    ReceivedChar = ScicRegs.SCIRXBUF.all;

	return ReceivedChar;
}

void SCIConsoleInit(void)
{
	//不需要 已经由GPIO复用 通过pinmux配置
//	InitScicGpio();

	Scic_fifo_init();

	Scic_echoback_init();
}

//===========================================================================
// No more.
//===========================================================================
