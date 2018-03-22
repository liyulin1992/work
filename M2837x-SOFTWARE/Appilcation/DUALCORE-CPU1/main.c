/*
 * main.c
 */
#include "F28x_Project.h"
#include "pinmux.h"
#include "F2837xD_Ipc_drivers.h"
#include "cla/cla_sqrt_shared.h"

//�����ⲿ��������
extern void CopyData();
extern void SCI_A();

#define LED1_TG GpioDataRegs.GPDTOGGLE.bit.GPIO111 = 1

#define BUFFER_SIZE 100

unsigned int pass;
unsigned int fail_cla;
float y[BUFFER_SIZE];

#pragma DATA_SECTION(fVal,"CpuToCla1MsgRAM");
float fVal;     //����
#pragma DATA_SECTION(fResult,"Cla1ToCpuMsgRAM")
float fResult;  //������

//flash.lib�ļ����е�ַ
extern Uint16 RamfuncsLoadStart_1;
extern Uint16 RamfuncsLoadSize_1;
extern Uint16 RamfuncsRunStart_1;

//extern __interrupt void Cla1Task1 ( void );

void Gpio_select(void);
void CLA_runTest(void);

void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;	// led1
    GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;

    GpioCtrlRegs.GPDMUX2.bit.GPIO112 = 0;   // led2
    GpioCtrlRegs.GPDDIR.bit.GPIO112 = 1;
    EDIS;
}

int main(void)
{
	memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
	memcpy(&RamfuncsRunStart_1, &RamfuncsLoadStart_1, (size_t)&RamfuncsLoadSize_1);

	// ���� boot ������� CPU2 ��ʼִ��Ӧ�ó���
	IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
    InitFlash();

    //һϵ�г�ʼ��
	InitSysCtrl();

	InitGpio();

	DINT;

	InitPieCtrl();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EINT;
	ERTM;

	//LED��GPIO��
	Gpio_select();

	//��ʼ��CAN��
	GPIO_setPinMuxConfig();

	//��ʼ��PWM��
	InitEPwmGpio();

	GPIO_SetupPinMux(112,GPIO_MUX_CPU2,0);

	//CLA����
	CLA_configClaMemory();
	CLA_initCpu1Cla1();

	EALLOW;

	//ѡ��PWM�Ŀ�������CPU1����2,�Ĵ������ֲ�
	DevCfgRegs.CPUSEL0.all = 0xff;

	//ADC����Ȩ
	DevCfgRegs.CPUSEL11.all = 0xf;

	//CAN����Ȩ
	DevCfgRegs.CPUSEL8.bit.CAN_A = 1;
	EDIS;

	//������ڴ����ж��е��á���Ҫ�û��Լ�ָ��������д
	CopyData();

	//��ʾ�����жϽ��ܣ�FLASH���������ڴ��ڽ����ж�����
	SCI_A();

	while(1)
	{
	    LED1_TG;
	    CLA_runTest();
	    DELAY_US(1000 * 500);
	}
}

void CLA_runTest(void)
{
    fVal = 2;

    Cla1ForceTask1andWait();

#if 0
    Cla1ForceTask2andWait();
    WAITSTEP;

    Cla1ForceTask3andWait();
    WAITSTEP;

    Cla1ForceTask4andWait();
    WAITSTEP;

    Cla1ForceTask5andWait();
    WAITSTEP;

    Cla1ForceTask6andWait();
    WAITSTEP;

    Cla1ForceTask7andWait();
    WAITSTEP;
#endif
}

