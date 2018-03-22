//###########################################################################
//
// FILE:   Example_2837xDSpi_FFDLB.c
//
// TITLE:  SPI Digital Loop Back program.
//
//! \addtogroup cpu01_example_list
//! <h1>SPI Digital Loop Back (spi_loopback)</h1>
//!
//!  This program uses the internal loop back test mode of the peripheral.
//!  Other then boot mode pin configuration, no other hardware configuration
//!  is required. Interrupts are not used.
//!
//!  A stream of data is sent and then compared to the received stream.
//!  The sent data looks like this: \n
//!  0000 0001 0002 0003 0004 0005 0006 0007 .... FFFE FFFF \n
//!  This pattern is repeated forever.
//!
//!  \b Watch \b Variables \n
//!  - \b sdata - sent data
//!  - \b rdata - received data
//!
//
//###########################################################################
// $TI Release: F2837xD Support Library v200 $
// $Release Date: Tue Jun 21 13:00:02 CDT 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "pinmux.h"
//
// Function Prototypes
//
void delay_loop(void);
void spi_xmit(Uint16 a);
void spi_fifo_init(void);
void spi_init(void);
void error(void);

#define SPI_BRR        ((200E6 / 4) / 1000E3) - 1

Uint16 sdata;  // send data
Uint16 rdata;  // received data

void main(void)
{
   InitSysCtrl();

//   InitSpiaGpio();
   GPIO_setPinMuxConfig();

   DINT;

   InitPieCtrl();

//
// Disable CPU __interrupts and clear all CPU __interrupt flags:
//
   IER = 0x0000;
   IFR = 0x0000;

//
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the __interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
//
   InitPieVectTable();

//
// Step 4. Initialize the Device Peripherals:
//
   spi_fifo_init();     // Initialize the Spi FIFO
   spi_init();          // init SPI

   sdata = 0x0001;
   for(;;)
   {

//        spi_xmit(sdata);

        //
        // Wait until data is received
        //
        while(SpicRegs.SPIFFRX.bit.RXFFST !=1) { }

        //
        // Check against sent data
        //
        rdata =SpicRegs.SPIRXBUF;
//        DELAY_US(10);
        spi_xmit(rdata);

   }
}

//
// delay_loop - Loop for a brief delay
//
void delay_loop()
{
    long i;
    for (i = 0; i < 1000; i++) {}
}

//
// error - Error function that halts the debugger
//
void error(void)
{
    asm("     ESTOP0");     // Test failed!! Stop!
    for (;;);
}

//
// spi_init - Initialize theSpic module settings
//
void spi_init()
{
   SpicRegs.SPICCR.all =0x000F;	 // Reset on, rising edge, 16-bit char bits
   SpicRegs.SPICCR.bit.SPILBK = 0;
   SpicRegs.SPICTL.all =0x0006;     // Enable master mode, normal phase,
                                     // enable talk, and SPI int disabled.
   SpicRegs.SPICTL.bit.MASTER_SLAVE = 0;
   SpicRegs.SPICCR.bit.CLKPOLARITY = 0;
   SpicRegs.SPICTL.bit.CLK_PHASE = 0;

   SpicRegs.SPIBRR.all =SPI_BRR;
   SpicRegs.SPICCR.all =0x009F;     // Relinquish SPI from Reset
   SpicRegs.SPIPRI.bit.FREE = 1;    // Set so breakpoints don't disturb
                                     // xmission
}

//
// spi_xmit - Transmit value via SPI
//
void spi_xmit(Uint16 a)
{
   SpicRegs.SPITXBUF = a;
}

//
// spi_fifo_init - Initialize SPI FIFO registers
//
void spi_fifo_init()
{
//	SpiaRegs.SPIFFTX.bit.SPIRST = 1;
//	SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;
//	SpiaRegs.SPIFFTX.bit.TXFFIENA = 1;
//
//	SpiaRegs.SPIFFRX.bit.RXFFIENA =1;

   SpicRegs.SPIFFTX.all = 0xE040;
   SpicRegs.SPIFFRX.all = 0x2044;
   SpicRegs.SPIFFCT.all = 0x0;
}

