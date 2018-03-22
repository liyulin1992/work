/*
 * cla_sqrt.c
 *
 *  Created on: 2017��6��29��
 *      Author: lrk
 */
#include "F28x_Project.h"

#include "cla_sqrt_shared.h"

void CLA_configClaMemory(void)
{
    extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    EALLOW;

    //���Ƶ�RAM������
    memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
           (uint32_t)&Cla1funcsLoadSize);

    //��ʼ��CLA1 TO CPU RAM
    MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

    //��ʼ��CPU TO CLA1 RAM
    MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};

    //LS3��LS4��LS5ΪCLA�����ʣ���Ϊ����ռ�
    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;

    //LS2��LS3��LS1��LS0ΪCLA�����ʣ���Ϊ���ݿռ�
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS2 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS2 = 0;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS3 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS3 = 0;

    EDIS;
}

//ӳ��CLA�����CLA�ж�
void CLA_initCpu1Cla1(void)
{
    EALLOW;
#ifdef CPU1
    // Enable CPU1.CLA1
    DevCfgRegs.DC1.bit.CPU1_CLA1 = 1;
    // Enable CPU2.CLA1
    DevCfgRegs.DC1.bit.CPU2_CLA1 = 1;
#endif //CPU1

    EDIS;

    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;

    //Send flag 5 to CPU2
//    IpcSync(5);

    //����Ĵ���˵��
    EALLOW;
    Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
//    Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
//    Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
//    Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
//    Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
//    Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
//    Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
//    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

    //����ʹ��ICAKָ����ʹ��CLA
    Cla1Regs.MCTL.bit.IACKE = 1;

    //ʹ���ж�
    Cla1Regs.MIER.bit.INT1 = 0x01;

    EDIS;

    EALLOW;
    //�������ʱ�ж�
    PieVectTable.CLA1_1_INT  = &cla1Isr1;
//    PieVectTable.CLA1_2_INT  = &cla1Isr2;
//    PieVectTable.CLA1_3_INT  = &cla1Isr3;
//    PieVectTable.CLA1_4_INT  = &cla1Isr4;
//    PieVectTable.CLA1_5_INT  = &cla1Isr5;
//    PieVectTable.CLA1_6_INT  = &cla1Isr6;
//    PieVectTable.CLA1_7_INT  = &cla1Isr7;
//    PieVectTable.CLA1_8_INT  = &cla1Isr8;

    //PIE�ж�
    PieCtrlRegs.PIEIER11.all= 0xffff;
    IER |= (M_INT11 );

    EDIS;
}

// cla1Isr1 - CLA1 ISR 1
__interrupt void cla1Isr1 ()
{
    // Acknowledge the end-of-task interrupt for task 1
    PieCtrlRegs.PIEACK.all = M_INT11;

    // Uncomment to halt debugger and stop here
//    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 2
__interrupt void cla1Isr2 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 3
__interrupt void cla1Isr3 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 4
__interrupt void cla1Isr4 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 5
__interrupt void cla1Isr5 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 6
__interrupt void cla1Isr6 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 7
__interrupt void cla1Isr7 ()
{
    asm(" ESTOP0");
}

// cla1Isr1 - CLA1 ISR 8
__interrupt void cla1Isr8 ()
{

    // Acknowledge the end-of-task interrupt for task 8
    PieCtrlRegs.PIEACK.all = M_INT11;

    // Uncomment to halt debugger and stop here
//    asm(" ESTOP0");
}
