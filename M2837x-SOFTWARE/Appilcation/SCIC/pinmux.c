//*****************************************************************************
//
// pinmux.c - Function to write the generated pin mux values to the appropriate
//            registers.
//            Created using TI Pinmux 3.0.625  on 2016骞�12鏈�20鏃� at 涓嬪崍3:55:10.
//
// NOTE!! This file uses bit field structures and macros from controlSUITE.
// This function is provided for your convenience and to serve as an example
// of the use of the generated header file, but its use is not required.
//
// To download controlSUITE:  http://www.ti.com/tool/controlsuite
//
//*****************************************************************************

#include "F28x_Project.h"
#include "pinmux.h"

//*****************************************************************************
//
// Configures the pin mux registers, using the generated register values.
//
// This function writes the values generated by the pin mux tool to their
// corresponding GPIO control registers. These generated values should be found
// in the generated "pinmux.h."
//
//*****************************************************************************
void
GPIO_setPinMuxConfig(void)
{
    Uint32 lockValA;
    Uint32 lockValB;
    Uint32 lockValC;
    Uint32 lockValD;
    Uint32 lockValE;
    Uint32 lockValF;

    EALLOW;

    //
    // Save the current value of the GPIO lock registers
    //
    lockValA = GpioCtrlRegs.GPALOCK.all;
    lockValB = GpioCtrlRegs.GPBLOCK.all;
    lockValC = GpioCtrlRegs.GPCLOCK.all;
    lockValD = GpioCtrlRegs.GPDLOCK.all;
    lockValE = GpioCtrlRegs.GPELOCK.all;
    lockValF = GpioCtrlRegs.GPFLOCK.all;

    //
    // Unlock the GPIO control registers
    //
    GpioCtrlRegs.GPALOCK.all = 0x00000000;
    GpioCtrlRegs.GPBLOCK.all = 0x00000000;
    GpioCtrlRegs.GPCLOCK.all = 0x00000000;
    GpioCtrlRegs.GPDLOCK.all = 0x00000000;
    GpioCtrlRegs.GPELOCK.all = 0x00000000;
    GpioCtrlRegs.GPFLOCK.all = 0x00000000;
    
    //
    // Write pin muxing to peripheral group mux registers
    //
    GpioCtrlRegs.GPAGMUX1.all  = GPAGMUX1_VALUE;
    GpioCtrlRegs.GPAGMUX2.all  = GPAGMUX2_VALUE;
    GpioCtrlRegs.GPBGMUX1.all  = GPBGMUX1_VALUE;
    GpioCtrlRegs.GPBGMUX2.all  = GPBGMUX2_VALUE;
    GpioCtrlRegs.GPCGMUX1.all  = GPCGMUX1_VALUE;
    GpioCtrlRegs.GPCGMUX2.all  = GPCGMUX2_VALUE;
    GpioCtrlRegs.GPDGMUX1.all  = GPDGMUX1_VALUE;
    GpioCtrlRegs.GPDGMUX2.all  = GPDGMUX2_VALUE;
    GpioCtrlRegs.GPEGMUX1.all  = GPEGMUX1_VALUE;
    GpioCtrlRegs.GPEGMUX2.all  = GPEGMUX2_VALUE;
    GpioCtrlRegs.GPFGMUX1.all  = GPFGMUX1_VALUE;
//    GpioCtrlRegs.GPFGMUX2.all  = GPFGMUX2_VALUE;

    //
    // Write pin muxing to mux registers
    //
    GpioCtrlRegs.GPAMUX1.all  = GPAMUX1_VALUE;
    GpioCtrlRegs.GPAMUX2.all  = GPAMUX2_VALUE;
    GpioCtrlRegs.GPBMUX1.all  = GPBMUX1_VALUE;
    GpioCtrlRegs.GPBMUX2.all  = GPBMUX2_VALUE;
    GpioCtrlRegs.GPCMUX1.all  = GPCMUX1_VALUE;
    GpioCtrlRegs.GPCMUX2.all  = GPCMUX2_VALUE;
    GpioCtrlRegs.GPDMUX1.all  = GPDMUX1_VALUE;
    GpioCtrlRegs.GPDMUX2.all  = GPDMUX2_VALUE;
    GpioCtrlRegs.GPEMUX1.all  = GPEMUX1_VALUE;
    GpioCtrlRegs.GPEMUX2.all  = GPEMUX2_VALUE;
    GpioCtrlRegs.GPFMUX1.all  = GPFMUX1_VALUE;
//    GpioCtrlRegs.GPFMUX2.all  = GPFMUX2_VALUE;

    //
    // Restore GPIO lock register values
    //
    GpioCtrlRegs.GPALOCK.all = lockValA;
    GpioCtrlRegs.GPBLOCK.all = lockValB;
    GpioCtrlRegs.GPCLOCK.all = lockValC;
    GpioCtrlRegs.GPDLOCK.all = lockValD;
    GpioCtrlRegs.GPELOCK.all = lockValE;
    GpioCtrlRegs.GPFLOCK.all = lockValF;

    EDIS;
}
