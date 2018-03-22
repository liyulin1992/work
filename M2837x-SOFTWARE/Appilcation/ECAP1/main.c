//###########################################################################
// $TI Release: F2837xD Support Library v200 $
// $Release Date: Tue Jun 21 13:00:02 CDT 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

// Included Files
#include "F28x_Project.h"
#include "SciStdio.h"
#include <stdio.h>

Uint32  ECap2IntCount;
Uint32  ECap2PassCount;
Uint32  EPwm3TimerDirection;
Uint32  CAP1 ,CAP2,CAP3,CAP4;
Uint32  Duty;
float 	T1,T2;
Uint16 p1;
int dir=1;

// Defines
// Configure the start/end period for the timer
#define PWM3_TIMER_MIN     10
#define PWM3_TIMER_MAX     8000
// To keep track of which way the timer value is moving
#define EPWM_TIMER_UP   1
#define EPWM_TIMER_DOWN 0

// Globals
Uint32  ECap1IntCount;
Uint32  ECap1PassCount;
Uint32  EPwm3TimerDirection;

unsigned int cap1,cap2;
unsigned int cap3,cap4;
unsigned int period1,period2,duty1,duty2;

// Function Prototypes
__interrupt void ecap1_isr(void);
void InitECapture(void);
void InitEPwmTimer(void);
void Fail(void);

void main(void)
{
	//系统初始化
   InitSysCtrl();

   InitEPwm3Gpio();
   InitECap1Gpio(14);
   GPIO_SetupPinOptions(14, GPIO_INPUT, GPIO_ASYNC);

   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   printf("Welcome to TL28377 ECAP Demo application.\r\n\r\n");

   DINT;
   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.ECAP1_INT = &ecap1_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

   //初始化配置
   InitEPwmTimer();    // For this example, only initialize the ePWM Timers
   InitECapture();

   ECap1IntCount = 0;
   ECap1PassCount = 0;

   //使能相关中断
   IER |= M_INT4;
   PieCtrlRegs.PIEIER4.bit.INTx1 = 1;

   //使能全局中断和调试模式
   EINT;
   ERTM;

   for(;;)
   {
      asm("          NOP");
   }
}

// InitEPwmTimer - Initialize ePWM3 timer configuration
void InitEPwmTimer()
{
   EALLOW;
   CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
   EDIS;

   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm3Regs.TBPRD = 100;
//   EPwm3Regs.CMPA.bit.CMPA = 500;
   EPwm3Regs.TBPHS.all = 0x00000000;
   EPwm3Regs.AQCTLA.bit.PRD = AQ_TOGGLE;      // Toggle on PRD
//   EPwm3Regs.AQCTLA.bit.CAU = AQ_TOGGLE;

   // TBCLK = SYSCLKOUT
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 1;
   EPwm3Regs.TBCTL.bit.CLKDIV = 0;

   EPwm3TimerDirection = EPWM_TIMER_UP;

   EALLOW;
   CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;
}

// InitECapture - Initialize ECAP1 configurations
void InitECapture()
{
   ECap1Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
   ECap1Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
   ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

   //
   // Configure peripheral registers
   //
   ECap1Regs.ECCTL2.bit.CONT_ONESHT = 1;   // One-shot
   ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;     // Stop at 4 events
   ECap1Regs.ECCTL1.bit.CAP1POL = 0;       // rising edge
   ECap1Regs.ECCTL1.bit.CAP2POL = 1;       // falling edge
   ECap1Regs.ECCTL1.bit.CAP3POL = 0;       // rising edge
   ECap1Regs.ECCTL1.bit.CAP4POL = 1;       // falling edge

   ECap1Regs.ECCTL1.bit.CTRRST1 = 1;       // Difference operation
   ECap1Regs.ECCTL1.bit.CTRRST2 = 1;       // Difference operation
   ECap1Regs.ECCTL1.bit.CTRRST3 = 1;       // Difference operation
   ECap1Regs.ECCTL1.bit.CTRRST4 = 1;       // Difference operation

   ECap1Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
   ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
   ECap1Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
   ECap1Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads
   ECap1Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt
}

//
// ecap1_isr - ECAP1 ISR
//             ECap runs twice as fast on F2837xD so multiplier
//             was increased from x2 to x4 and the fudge factor was
//             increased from 1 to 4.
//              Cap input is syc'ed to SYSCLKOUT so there may be
//              a +/- 1 cycle variation
//
__interrupt void ecap1_isr(void)
{
//   unsigned int cap1,cap2;
//   unsigned int cap3,cap4;

  

//   printf("The period : %d\n",cap2-cap1);
#if 0
   if(ECap1Regs.CAP2 > EPwm3Regs.TBPRD*4+4 ||
      ECap1Regs.CAP2 < EPwm3Regs.TBPRD*4-4)
   {
       Fail();
   }

   if(ECap1Regs.CAP3 > EPwm3Regs.TBPRD*4+4 ||
      ECap1Regs.CAP3 < EPwm3Regs.TBPRD*4-4)
   {
       Fail();
   }

   if(ECap1Regs.CAP4 > EPwm3Regs.TBPRD*4+4 ||
      ECap1Regs.CAP4 < EPwm3Regs.TBPRD*4-4)
   {
       Fail();
   }
#endif
   ECap1IntCount++;

#if 0
   if(EPwm3TimerDirection == EPWM_TIMER_UP)
   {
        if(EPwm3Regs.TBPRD < PWM3_TIMER_MAX)
        {
           EPwm3Regs.TBPRD++;
        }
        else
        {
           EPwm3TimerDirection = EPWM_TIMER_DOWN;
           EPwm3Regs.TBPRD--;
        }
   }

   else
   {
        if(EPwm3Regs.TBPRD > PWM3_TIMER_MIN)
        {
           EPwm3Regs.TBPRD--;
        }
        else
        {
           EPwm3TimerDirection = EPWM_TIMER_UP;
           EPwm3Regs.TBPRD++;
        }
   }
#endif
   CAP1 = ECap2Regs.CAP1;
   CAP2 = ECap2Regs.CAP2;
   CAP3 = ECap2Regs.CAP3;
   CAP4 = ECap2Regs.CAP4;
   
   T1 =(float) (200000 / (CAP3 + CAP4));
   p1 = (200000 / (CAP3 + CAP4));
   SCIprintf("The waveform period is %d KHz\n",p1);
   
   ECap1PassCount++;

   ECap1Regs.ECCLR.bit.CEVT4 = 1;
   ECap1Regs.ECCLR.bit.INT = 1;
   ECap1Regs.ECCTL2.bit.REARM = 1;

   // Acknowledge this __interrupt to receive more __interrupts from group 4
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

// Fail - Function that halts debugger
void Fail()
{
   asm("   ESTOP0");
}
