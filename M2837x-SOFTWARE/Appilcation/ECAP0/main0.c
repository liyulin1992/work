/**
 * ---------------------------------------------------------------------------------------
 * 			广州创龙电子科技有限公司
 *
 * 			Copyright 2016 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *     硬件连接：在EPMW3A产生PWM，接到J8的GPIO13。
 *	   将GPIO13设置为捕获输入模式，通过读取ECAP计数寄存器的值再转换成实际频率。
 *	   打印的结果单位为KHz，用户也可以在变量窗口察看浮点数结果（T1、T2）。
 *    	
 * ---------------------------------------------------------------------------------------
 * 时间：2017-1-1
 * ---------------------------------------------------------------------------------------
 */
// Included Files
#include "F28x_Project.h"
#include "SciStdio.h"

//宏，配置PWM周期的参数
#define PWM_PERIOD 		   100

Uint32  ECap2IntCount;
Uint32  EPwm3TimerDirection;
Uint32  CAP1 ,CAP2,CAP3,CAP4;
Uint32  Duty;
float 	T1,T2;
Uint16 p1;
int dir=1;

//函数声明
__interrupt void ECap2_isr(void);
void InitECapture(void);
void InitEPwmTimer(void);
void Fail(void);

void main(void)
{

   //初始化系统
   InitSysCtrl();

   //初始化GPIO口
   InitEPwm3Gpio();
   InitECap2Gpio(13);
   GPIO_SetupPinOptions(13, GPIO_INPUT, GPIO_ASYNC);

   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   SCIPuts("Welcome to TL28377 ECAP Demo application.\r\n\r\n",-1);

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   //初始化中断向量表
   InitPieVectTable();

   //注册中断服务子函数
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.ECAP2_INT = &ECap2_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

   //初始化外设相关配置
   InitEPwmTimer();    
   InitECapture();

   ECap2IntCount = 0;

   //使能相关中断
   IER |= M_INT4;
   PieCtrlRegs.PIEIER4.bit.INTx2 = 1;

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global __interrupt INTM
   ERTM;   // Enable Global realtime __interrupt DBGM

   for(;;)
   {
      asm("          NOP");
   }
}

//
void InitEPwmTimer()
{
   EALLOW;
   CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
   EDIS;

   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // 计数模式
   EPwm3Regs.TBPRD = PWM_PERIOD;
   EPwm3Regs.TBPHS.all = 0x00000000;
   EPwm3Regs.AQCTLA.bit.PRD = AQ_TOGGLE;      // 触发方式

   // TBCLK = SYSCLKOUT
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 1;
   EPwm3Regs.TBCTL.bit.CLKDIV = 0;

   EPwm3TimerDirection = 1;

   EALLOW;
   CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;
}

//初始化ECAP相关配置
void InitECapture()
{
   ECap2Regs.ECEINT.all = 0x0000;          // Disable all capture __interrupts
   ECap2Regs.ECCLR.all = 0xFFFF;           // Clear all CAP __interrupt flags
   ECap2Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
   ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

   //运行模式，以及捕获极性配置
   ECap2Regs.ECCTL2.bit.CONT_ONESHT = 1;   // One-shot
   ECap2Regs.ECCTL2.bit.STOP_WRAP = 3;     // Stop at 4 events
   ECap2Regs.ECCTL1.bit.CAP1POL = 0;       // Falling edge
   ECap2Regs.ECCTL1.bit.CAP2POL = 1;       // Rising edge
   ECap2Regs.ECCTL1.bit.CAP3POL = 0;       // Falling edge
   ECap2Regs.ECCTL1.bit.CAP4POL = 1;       // Rising edge
   
   //差分模式
   ECap2Regs.ECCTL1.bit.CTRRST1 = 1;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST2 = 1;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST3 = 1;       // Difference operation
   ECap2Regs.ECCTL1.bit.CTRRST4 = 1;       // Difference operation
   
   ECap2Regs.ECCTL2.bit.SYNCI_EN = 1;      // Enable sync in
   ECap2Regs.ECCTL2.bit.SYNCO_SEL = 0;     // Pass through
   ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

   ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
   ECap2Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
   ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable CAP1-CAP4 register loads
   ECap2Regs.ECEINT.bit.CEVT4 = 1;         // 4 events = __interrupt
}

//ECAP中断服务子函数，计算频率并且通过串口打印
__interrupt void ECap2_isr(void)
{

   CAP1 = ECap2Regs.CAP1;
   CAP2 = ECap2Regs.CAP2;
   CAP3 = ECap2Regs.CAP3;
   CAP4 = ECap2Regs.CAP4;

   T1 =(float) (200000 / (CAP3 + CAP4));
   p1 = (200000 / (CAP3 + CAP4));
   SCIprintf("The waveform period is %d KHz\n",p1);

   ECap2IntCount++;

   ECap2Regs.ECCLR.bit.CEVT4 = 1;
   ECap2Regs.ECCLR.bit.INT = 1;
   ECap2Regs.ECCTL2.bit.REARM = 1;

   // Acknowledge this __interrupt to receive more __interrupts from group 4
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;

}
