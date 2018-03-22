//*****************************************************************************
//
//  pinmux.h - Created using TI Pinmux 3.0.625  on 2017年1月3日 at 下午4:21:02.
//
//*****************************************************************************
//
//  These values will provide the functionality requested when written into
//  the registers for which the #defines are named.  For example, using the
//  controlSUITE device support header files, use the defines like in this
//  sample function:
//
//  void samplePinMuxFxn(void)
//  {
//      EALLOW;
//      //
//      // Write generated values to mux registers
//      //
//      GpioCtrlRegs.GPAMUX1.all  = GPAMUX1_VALUE;
//      GpioCtrlRegs.GPAMUX2.all  = GPAMUX2_VALUE;
//      GpioCtrlRegs.GPBMUX1.all  = GPBMUX1_VALUE;
//        . . .
//      EDIS;
//  }
//
//  NOTE:  These GPIO control registers are only available on CPU1.
//
//*****************************************************************************

//
// Port A mux register values
//
#define GPAMUX1_VALUE		0x00000000
#define GPAMUX2_VALUE		0x00000000
#define GPAGMUX1_VALUE		0x00000000
#define GPAGMUX2_VALUE		0x00000000

//
// Port B mux register values
//
#define GPBMUX1_VALUE		0x00000000
#define GPBMUX2_VALUE		0x00000000
#define GPBGMUX1_VALUE		0x00000000
#define GPBGMUX2_VALUE		0x00000000

//
// Port C mux register values
//
#define GPCMUX1_VALUE		0x00000000
#define GPCMUX2_VALUE		0x00000000
#define GPCGMUX1_VALUE		0x00000000
#define GPCGMUX2_VALUE		0x00000000

//
// Port D mux register values
//
// Pin T8 (GPIO122) to SPISIMOC (mode 6)
// Pin U8 (GPIO123) to SPISOMIC (mode 6)
// Pin V8 (GPIO124) to SPICLKC (mode 6)
// Pin T9 (GPIO125) to SPISTEC (mode 6)
#define GPDMUX1_VALUE		0x00000000
#define GPDMUX2_VALUE		0x0aa00000
#define GPDGMUX1_VALUE		0x00000000
#define GPDGMUX2_VALUE		0x05500000

//
// Port E mux register values
//
#define GPEMUX1_VALUE		0x00000000
#define GPEMUX2_VALUE		0x00000000
#define GPEGMUX1_VALUE		0x00000000
#define GPEGMUX2_VALUE		0x00000000

//
// Port F mux register values
//
#define GPFMUX1_VALUE		0x00000000
#define GPFMUX2_VALUE		0x00000000
#define GPFGMUX1_VALUE		0x00000000
#define GPFGMUX2_VALUE		0x00000000

//*****************************************************************************
//
// Function prototype for function to write values above into their
// corresponding registers. This function is found in pinmux.c. Its use is
// completely optional.
//
//*****************************************************************************

extern void GPIO_setPinMuxConfig(void);
