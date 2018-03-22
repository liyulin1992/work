/*
 * hbhk_pwm.c
 *
 *  Created on: 2017年2月24日
 *      Author: liyulin
 */
#include "hbhk_pwm.h"

#include "F28x_Project.h"

//声明结构体
EPWM_INFO epwm1_info;
EPWM_INFO epwm2_info;
EPWM_INFO epwm3_info;
EPWM_INFO epwm4_info;

interrupt void epwm1_isr(void)
{
//    intcount++;

    //更新寄存器的CPM值
    update_compare(&epwm1_info);

    //清楚中断标志位
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

interrupt void epwm2_isr(void)
{
    //更新寄存器的CPM值
    update_compare(&epwm2_info);

    //清楚中断标志位
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

interrupt void  epwm3_isr(void)
{
    //更新寄存器的CPM值
    update_compare(&epwm3_info);

    //清楚中断标志位
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

interrupt void epwm4_isr(void)
{
    //更新寄存器的CPM值
    update_compare(&epwm4_info);

    //清楚中断标志位
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


// update_compare - Update the compare values for the specified EPWM
void update_compare(EPWM_INFO *epwm_info)
{

   //每10次中断更改寄存器值
   if(epwm_info->EPwmTimerIntCount == 10)
   {
       epwm_info->EPwmTimerIntCount = 0;

       //检测CMPA是否达到最大，是否改变方向
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

       //检测CMPA是否达到最小，是否改变方向  //
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

       //CMPB
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

       //CMPB
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

//使能中断
void Int_Pwm_Enable()
{
	IER |= M_INT3;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx3 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx4 = 1;
}

//注册中断向量表
void Pie_Register_Pwm()
{
	//注册中断
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_INT = &epwm1_isr;
    PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.EPWM3_INT = &epwm3_isr;
    PieVectTable.EPWM4_INT = &epwm4_isr;
    EDIS;
}

//初始化配置
void InitEPwm1Example()
{
	GpioDataRegs.GPDSET.bit.GPIO112 = 1;
   // 配置时基
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   // Setup shadow register load on ZERO
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set Compare values
   EPwm1Regs.CMPA.bit.CMPA = EPWM1_MIN_CMPA;     // Set compare A value
   EPwm1Regs.CMPB.bit.CMPB = EPWM1_MIN_CMPB;     // Set Compare B value

   //动作模式
   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A,
                                                 // up count
   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B,
                                                 // up count
   //中断
   EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
   EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
   EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

   //给配置参数的结构体赋值
   epwm1_info.EPwm_CMPA_Direction = EPWM_CMP_UP; // Start by increasing
                                                 // CMPA & CMPB
   epwm1_info.EPwm_CMPB_Direction = EPWM_CMP_UP;
   epwm1_info.EPwmTimerIntCount = 0;             // Zero the interrupt counter
   epwm1_info.EPwmRegHandle = &EPwm1Regs;        // Set the pointer to the
                                                 // ePWM module
   epwm1_info.EPwmMaxCMPA = EPWM1_MAX_CMPA;      // Setup min/max
                                                 // CMPA/CMPB values
   epwm1_info.EPwmMinCMPA = EPWM1_MIN_CMPA;
   epwm1_info.EPwmMaxCMPB = EPWM1_MAX_CMPB;
   epwm1_info.EPwmMinCMPB = EPWM1_MIN_CMPB;
}
#if 1
void InitEPwm2Example()
{
	// 配置时基
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;       // Set timer period
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   //中断
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set Compare values
   EPwm2Regs.CMPA.bit.CMPA = EPWM2_MIN_CMPA;      // Set compare A value
   EPwm2Regs.CMPB.bit.CMPB = EPWM2_MAX_CMPB;      // Set Compare B value

   // Set actions
   EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;            // Clear PWM2A on Period
   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;              // Set PWM2A on event A,
                                                   // up count

   EPwm2Regs.AQCTLB.bit.PRD = AQ_CLEAR;            // Clear PWM2B on Period
   EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;              // Set PWM2B on event B,
                                                   // up count

   // Interrupt where we will change the Compare Values
   EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm2Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event

   //给配置参数的结构体赋值
   epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN; // and decreasing CMPB
   epwm2_info.EPwmTimerIntCount = 0;               // Zero the interrupt
                                                   // counter
   epwm2_info.EPwmRegHandle = &EPwm2Regs;          // Set the pointer to the
                                                   // ePWM module
   epwm2_info.EPwmMaxCMPA = EPWM2_MAX_CMPA;        // Setup min/max
                                                   // CMPA/CMPB values
   epwm2_info.EPwmMinCMPA = EPWM2_MIN_CMPA;
   epwm2_info.EPwmMaxCMPB = EPWM2_MAX_CMPB;
   epwm2_info.EPwmMinCMPB = EPWM2_MIN_CMPB;
}

void InitEPwm3Example(void)
{
	//配置时基
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;       // Set timer period
   EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

   // Setup shadow register load on ZERO
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   EPwm3Regs.CMPA.bit.CMPA = EPWM3_MIN_CMPA;  // Set compare A value
   EPwm3Regs.CMPB.bit.CMPB = EPWM3_MAX_CMPB;  // Set Compare B value

   // Set Actions
   EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM3A on event B, up count
   EPwm3Regs.AQCTLA.bit.CBU = AQ_CLEAR;       // Clear PWM3A on event B,
                                              // up count
   EPwm3Regs.AQCTLB.bit.ZRO = AQ_TOGGLE;      // Toggle EPWM3B on Zero

   //中断
   EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  // Select INT on Zero event
   EPwm3Regs.ETSEL.bit.INTEN = 1;             // Enable INT
   EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;        // Generate INT on 3rd event

   //给配置参数的结构体赋值
   epwm3_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
   epwm3_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;

   epwm3_info.EPwmTimerIntCount = 0;
   epwm3_info.EPwmRegHandle = &EPwm3Regs;
   epwm3_info.EPwmMaxCMPA = EPWM3_MAX_CMPA;
   epwm3_info.EPwmMinCMPA = EPWM3_MIN_CMPA;
   epwm3_info.EPwmMaxCMPB = EPWM3_MAX_CMPB;
   epwm3_info.EPwmMinCMPB = EPWM3_MIN_CMPB;
}

void InitEPwm4Example(void)
{
	 EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	 EPwm4Regs.TBPRD = EPWM4_TIMER_TBPRD;       // Set timer period
	 EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	 EPwm4Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
	 EPwm4Regs.TBCTR = 0x0000;                  // Clear counter
	 EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;   // Clock ratio to SYSCLKOUT
	 EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;


	 EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	 EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	 EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	 EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	 //CMP
	 EPwm4Regs.CMPA.bit.CMPA = EPWM4_MIN_CMPA;  // Set compare A value
	 EPwm4Regs.CMPB.bit.CMPB = EPWM4_MAX_CMPB;  // Set Compare B value

	 //动作
	 EPwm4Regs.AQCTLA.bit.CAU = AQ_SET;         // Set PWM3A on event B, up count
	 EPwm4Regs.AQCTLA.bit.CBU = AQ_CLEAR;       // Clear PWM3A on event B,
	                                              // up count
	 EPwm4Regs.AQCTLB.bit.ZRO = AQ_TOGGLE;      // Toggle EPWM3B on Zero

	 //配置中断
	 EPwm4Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;  // Select INT on Zero event
	 EPwm4Regs.ETSEL.bit.INTEN = 1;             // Enable INT
	 EPwm4Regs.ETPS.bit.INTPRD = ET_3RD;        // Generate INT on 3rd event

	 //给配置参数的结构体赋值
	 epwm4_info.EPwm_CMPA_Direction = EPWM_CMP_UP;
	 epwm4_info.EPwm_CMPB_Direction = EPWM_CMP_DOWN;

	 epwm4_info.EPwmTimerIntCount = 0;
	 epwm4_info.EPwmRegHandle = &EPwm4Regs;
	 epwm4_info.EPwmMaxCMPA = EPWM4_MAX_CMPA;
	 epwm4_info.EPwmMinCMPA = EPWM4_MIN_CMPA;
	 epwm4_info.EPwmMaxCMPB = EPWM4_MAX_CMPB;
	 epwm4_info.EPwmMinCMPB = EPWM4_MIN_CMPB;
}

void Init_Pwm()
{
	InitEPwm1Example();
	InitEPwm2Example();
	InitEPwm3Example();
	InitEPwm4Example();
}
#endif
