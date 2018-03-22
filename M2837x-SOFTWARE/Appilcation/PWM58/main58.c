//###########################################################################
//
// FILE:   epwm_up_aq_cpu01.c
//
// TITLE:  Action Qualifier Module - Using up count.
//
//! \addtogroup cpu01_example_list
//! <h1> EPWM Action Qualifier (epwm_up_aq)</h1>
//!
//! This example configures EPwm5, EPwm6, ePWM3 to produce an
//! waveform with independent modulation on EPWMxA and
//! EPWMxB.
//!
//! The compare values CMPA and CMPB are modified within the ePWM's ISR.
//!
//! The TB counter is in up count mode for this example.
//!
//! View the EPwm5A/B(PA0_GPIO0 & PA1_GPIO1), EPwm6A/B(PA2_GPIO2 & PA3_GPIO3)
//! and EPWM3A/B(PA4_GPIO4 & PA5_GPIO5) waveforms via an oscilloscope.
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

//
// Defines
//
#define EPwm5_TIMER_TBPRD  2000  // Period register
#define EPwm5_MAX_CMPA     1950
#define EPwm5_MIN_CMPA       50
#define EPwm5_MAX_CMPB     1950
#define EPwm5_MIN_CMPB       50

#define EPwm6_TIMER_TBPRD  2000  // Period register
#define EPwm6_MAX_CMPA     1950
#define EPwm6_MIN_CMPA       50
#define EPwm6_MAX_CMPB     1950
#define EPwm6_MIN_CMPB       50

#define EPWM3_TIMER_TBPRD  2000  // Period register
#define EPWM3_MAX_CMPA      950
#define EPWM3_MIN_CMPA       50
#define EPWM3_MAX_CMPB     1950
#define EPWM3_MIN_CMPB     1050

#define EPWM8_TIMER_TBPRD  2000
#define EPWM8_MAX_CMPA    	950
#define EPWM8_MIN_CMPA       50
#define EPWM8_MAX_CMPB     1950
#define EPWM8_MIN_CMPB     1050

#define EPWM_CMP_UP           1
#define EPWM_CMP_DOWN         0

//
// Globals
//
typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
}EPWM_INFO;

EPWM_INFO epwm5_info;
EPWM_INFO epwm6_info;
EPWM_INFO epwm7_info;
EPWM_INFO epwm8_info;

//
//  Function Prototypes
//
void InitEPwm5Example(void);
void InitEPwm6Example(void);
void InitEPwm7Example(void);
void InitEPwm8Example(void);

__interrupt void epwm5_isr(void);
__interrupt void epwm6_isr(void);
__interrupt void epwm7_isr(void);
__interrupt void epwm8_isr(void);
void update_compare(EPWM_INFO*);

//
// Main
//
void main(void)
{
//
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xD_SysCtrl.c file.
//
    InitSysCtrl();

//
// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
//
//    InitGpio();

//
// Enable PWM1, PWM2 and PWM3
//
//    CpuSysRegs.PCLKCR2.bit.EPwm5=1;
//    CpuSysRegs.PCLKCR2.bit.EPwm6=1;
//    CpuSysRegs.PCLKCR2.bit.EPWM3=1;
//
//    CpuSysRegs.PCLKCR2.bit.EPWM8=1;
    CpuSysRegs.PCLKCR2.bit.EPWM5=1;
    CpuSysRegs.PCLKCR2.bit.EPWM6=1;
    CpuSysRegs.PCLKCR2.bit.EPWM7=1;
    CpuSysRegs.PCLKCR2.bit.EPWM8=1;

//
// For this case just init GPIO pins for EPwm5, EPwm6, ePWM3
// These functions are in the F2837xD_EPwm.c file
//
//    InitEPwm5Gpio();
//    InitEPwm6Gpio();
//    InitEPwm3Gpio();
//    InitEPWM8Gpio();
    InitEPwm5Gpio();
    InitEPwm6Gpio();
    InitEPwm7Gpio();
    InitEPwm8Gpio();
    //
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
//
    DINT;

//
// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
//
    InitPieCtrl();

//
// Disable CPU interrupts and clear all CPU interrupt flags:
//
    IER = 0x0000;
    IFR = 0x0000;

//
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
//
    InitPieVectTable();

//
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
//
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM5_INT = &epwm5_isr;
    PieVectTable.EPWM6_INT = &epwm6_isr;
    PieVectTable.EPWM7_INT = &epwm7_isr;
    PieVectTable.EPWM8_INT = &epwm8_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

//
// For this example, only initialize the ePWM
//
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm5Example();
    InitEPwm6Example();
    InitEPwm7Example();
    InitEPwm8Example();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

//
// Step 4. User specific code, enable interrupts:
//
// Enable CPU INT3 which is connected to EPwm5-3 INT:
//
    IER |= M_INT3;

//
// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
//
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx4 = 1;
//
// Enable global Interrupts and higher priority real-time debug events:
//
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

//
// Step 5. IDLE loop. Just sit and loop forever (optional):
//
    for(;;)
    {
        asm ("  NOP");
    }
}

//
// EPwm5_isr - EPwm5 ISR to update compare values
//
__interrupt void epwm5_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm5_info);

    //
    // Clear INT flag for this timer
    //
    EPwm5Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// EPwm6_isr - EPwm6 ISR to update compare values
//
__interrupt void epwm6_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm6_info);

    //
    // Clear INT flag for this timer
    //
    EPwm6Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm3_isr - EPWM3 ISR to update compare values
//
__interrupt void epwm7_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm7_info);

    //
    // Clear INT flag for this timer
    //
    EPwm3Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void epwm8_isr(void)
{
	 update_compare(&epwm8_info);

	 EPwm8Regs.ETCLR.bit.INT = 1;

	 PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

//
// InitEPwm5Example - Initialize EPwm5 values
//
void InitEPwm5Example()
{
   //
   // Setup TBCLK
   //
   EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm5Regs.TBPRD = EPwm5_TIMER_TBPRD;       // Set timer period
   EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm5Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm5Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   //
   // Setup shadow register load on ZERO
   //
   EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm5Regs.CMPA.bit.CMPA = EPwm5_MIN_CMPA;     // Set compare A value
   EPwm5Regs.CMPB.bit.CMPB = EPwm5_MIN_CMPB;     // Set Compare B value

   //
   // Set actions
   //
   EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
   EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A,
                                                 // up count

   EPwm5Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
   EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B,
                                                 // up count

   //
   // Interrupt where we will change the Compare Values
   //
   EPwm5Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
   EPwm5Regs.ETSEL.bit.INTEN = 1;                // Enable INT
   EPwm5Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //
   epwm5_info.EPwm_CMPA_Direction = EPWM_CMP_UP; // Start by increasing
                                                 // CMPA & CMPB
   epwm5_info.EPwm_CMPB_Direction = EPWM_CMP_UP;
   epwm5_info.EPwmTimerIntCount = 0;             // Zero the interrupt counter
   epwm5_info.EPwmRegHandle = &EPwm5Regs;        // Set the pointer to the
                                                 // ePWM module
   epwm5_info.EPwmMaxCMPA = EPwm5_MAX_CMPA;      // Setup min/max
                                                 // CMPA/CMPB values
   epwm5_info.EPwmMinCMPA = EPwm5_MIN_CMPA;
   epwm5_info.EPwmMaxCMPB = EPwm5_MAX_CMPB;
   epwm5_info.EPwmMinCMPB = EPwm5_MIN_CMPB;
}

//
// InitEPwm6Example - Initialize EPwm6 values
//
void InitEPwm6Example()
{
   //
   // Setup TBCLK
   //
   EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm6Regs.TBPRD = EPwm6_TIMER_TBPRD;       // Set timer period
   EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm6Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm6Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   //
   // Setup shadow register load on ZERO
   //
   EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm6Regs.CMPA.bit.CMPA = EPwm6_MIN_CMPA;      // Set compare A value
   EPwm6Regs.CMPB.bit.CMPB = EPwm6_MAX_CMPB;      // Set Compare B value

   //
   // Set actions
   //
   EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Clear PWM2A on Period
   EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;              // Set PWM2A on event A,
                                                   // up count

   EPwm6Regs.AQCTLB.bit.PRD = AQ_CLEAR;            // Clear PWM2B on Period
   EPwm6Regs.AQCTLB.bit.CBU = AQ_SET;              // Set PWM2B on event B,
                                                   // up count

   //
   // Interrupt where we will change the Compare Values
   //
   EPwm6Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm6Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm6Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //
   epwm6_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm6_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN; // and decreasing CMPB
   epwm6_info.EPwmTimerIntCount = 0;               // Zero the interrupt
                                                   // counter
   epwm6_info.EPwmRegHandle = &EPwm6Regs;          // Set the pointer to the
                                                   // ePWM module
   epwm6_info.EPwmMaxCMPA = EPwm6_MAX_CMPA;        // Setup min/max
                                                   // CMPA/CMPB values
   epwm6_info.EPwmMinCMPA = EPwm6_MIN_CMPA;
   epwm6_info.EPwmMaxCMPB = EPwm6_MAX_CMPB;
   epwm6_info.EPwmMinCMPB = EPwm6_MIN_CMPB;
}

//
// InitEPwm3Example - Initialize EPWM3 values
//
void InitEPwm7Example(void)
{
   //
   // Setup TBCLK
   //
   EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm7Regs.TBPRD = EPWM3_TIMER_TBPRD;       // Set timer period
   EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm7Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm7Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
   EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;

   //
   // Setup shadow register load on ZERO
   //
   EPwm7Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm7Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm7Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm7Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm7Regs.CMPA.bit.CMPA = EPWM3_MIN_CMPA;  // Set compare A value
   EPwm7Regs.CMPB.bit.CMPB = EPWM3_MAX_CMPB;  // Set Compare B value

   //
   // Set Actions
   //
   EPwm7Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM3A on event B, up count
   EPwm7Regs.AQCTLA.bit.CBU = AQ_CLEAR;       // Clear PWM3A on event B,
                                              // up count
   EPwm7Regs.AQCTLB.bit.ZRO = AQ_TOGGLE;      // Toggle EPWM3B on Zero

   //
   // Interrupt where we will change the Compare Values
   //
   EPwm7Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  // Select INT on Zero event
   EPwm7Regs.ETSEL.bit.INTEN = 1;             // Enable INT
   EPwm7Regs.ETPS.bit.INTPRD = ET_3RD;        // Generate INT on 3rd event

   //
   // Start by increasing the compare A and decreasing compare B
   //
   epwm7_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
   epwm7_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;

   //
   // Start the count at 0
   //
   epwm7_info.EPwmTimerIntCount = 0;
   epwm7_info.EPwmRegHandle = &EPwm3Regs;
   epwm7_info.EPwmMaxCMPA = EPWM3_MAX_CMPA;
   epwm7_info.EPwmMinCMPA = EPWM3_MIN_CMPA;
   epwm7_info.EPwmMaxCMPB = EPWM3_MAX_CMPB;
   epwm7_info.EPwmMinCMPB = EPWM3_MIN_CMPB;
}

void InitEPwm8Example(void)
{
	 EPwm8Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	 EPwm8Regs.TBPRD = EPWM8_TIMER_TBPRD;       // Set timer period
	 EPwm8Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	 EPwm8Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
	 EPwm8Regs.TBCTR = 0x0000;                  // Clear counter
	 EPwm8Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
	 EPwm8Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	 //
	 // Setup shadow register load on ZERO
	 //
	 EPwm8Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	 EPwm8Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	 EPwm8Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	 EPwm8Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	 //
	 // Set Compare values
	 //
	 EPwm8Regs.CMPA.bit.CMPA = EPWM8_MIN_CMPA;  // Set compare A value
	 EPwm8Regs.CMPB.bit.CMPB = EPWM8_MAX_CMPB;  // Set Compare B value

	 //
	 // Set Actions
	 //
	 EPwm8Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM3A on event B, up count
	 EPwm8Regs.AQCTLA.bit.CBU = AQ_CLEAR;       // Clear PWM3A on event B,
	                                              // up count
	 EPwm8Regs.AQCTLB.bit.ZRO = AQ_TOGGLE;      // Toggle EPWM3B on Zero

	 //
	 // Interrupt where we will change the Compare Values
	 //
	 EPwm8Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  // Select INT on Zero event
	 EPwm8Regs.ETSEL.bit.INTEN = 1;             // Enable INT
	 EPwm8Regs.ETPS.bit.INTPRD = ET_3RD;        // Generate INT on 3rd event

	 //
	 // Start by increasing the compare A and decreasing compare B
	 //
	 epwm8_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
	 epwm8_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;

	 //
	 // Start the count at 0
	 //
	 epwm8_info.EPwmTimerIntCount = 0;
	 epwm8_info.EPwmRegHandle = &EPwm8Regs;
	 epwm8_info.EPwmMaxCMPA = EPWM8_MAX_CMPA;
	 epwm8_info.EPwmMinCMPA = EPWM8_MIN_CMPA;
	 epwm8_info.EPwmMaxCMPB = EPWM8_MAX_CMPB;
	 epwm8_info.EPwmMinCMPB = EPWM8_MIN_CMPB;
}

//
// update_compare - Update the compare values for the specified EPWM
//
void update_compare(EPWM_INFO *epwm_info)
{
   //
   // Every 10'th interrupt, change the CMPA/CMPB values
   //
   if(epwm_info->EPwmTimerIntCount == 10)
   {
       epwm_info->EPwmTimerIntCount = 0;

       //
       // If we were increasing CMPA, check to see if
       // we reached the max value.  If not, increase CMPA
       // else, change directions and decrease CMPA
       //
       if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
       {
           if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < epwm_info->EPwmMaxCMPA)
           {
              epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;
           }
           else
           {
              epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
              epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;
           }
       }

       //
       // If we were decreasing CMPA, check to see if
       // we reached the min value.  If not, decrease CMPA
       // else, change directions and increase CMPA
       //
       else
       {
           if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == epwm_info->EPwmMinCMPA)
           {
              epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
              epwm_info->EPwmRegHandle->CMPA.bit.CMPA++;
           }
           else
           {
              epwm_info->EPwmRegHandle->CMPA.bit.CMPA--;
           }
       }

       //
       // If we were increasing CMPB, check to see if
       // we reached the max value.  If not, increase CMPB
       // else, change directions and decrease CMPB
       //
       if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
       {
           if(epwm_info->EPwmRegHandle->CMPB.bit.CMPB < epwm_info->EPwmMaxCMPB)
           {
              epwm_info->EPwmRegHandle->CMPB.bit.CMPB++;
           }
           else
           {
              epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
              epwm_info->EPwmRegHandle->CMPB.bit.CMPB--;
           }
       }

       //
       // If we were decreasing CMPB, check to see if
       // we reached the min value.  If not, decrease CMPB
       // else, change directions and increase CMPB
       //
       else
       {
           if(epwm_info->EPwmRegHandle->CMPB.bit.CMPB ==
              epwm_info->EPwmMinCMPB)
           {
              epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;
              epwm_info->EPwmRegHandle->CMPB.bit.CMPB++;
           }
           else
           {
              epwm_info->EPwmRegHandle->CMPB.bit.CMPB--;
           }
       }
   }

   else
   {
      epwm_info->EPwmTimerIntCount++;
   }

   return;
}

