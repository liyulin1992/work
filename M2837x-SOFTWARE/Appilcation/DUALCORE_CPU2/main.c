#include "F28x_Project.h"

#include "drv/hbhk_adc.h"
#include "drv/hbhk_pwm.h"
#include "cla/cla_sqrt_shared.h"

#define LED2_TG GpioDataRegs.GPDTOGGLE.bit.GPIO112 = 1

#define BUFFER_SIZE 100

unsigned int pass;
unsigned int fail_cla;
float y[BUFFER_SIZE];

#pragma DATA_SECTION(fVal,"CpuToCla1MsgRAM");
float fVal;     //输入
#pragma DATA_SECTION(fResult,"Cla1ToCpuMsgRAM")
float fResult;  //输出结果

//引用外部函数声明
extern void CopyData();

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadSize;
extern Uint16 RamfuncsRunStart;

//flash.lib文件运行地址
extern Uint16 RamfuncsLoadStart_1;
extern Uint16 RamfuncsLoadSize_1;
extern Uint16 RamfuncsRunStart_1;

extern void can_init(void);
extern void can_loop(void);

extern interrupt void epwm1_isr(void);
extern interrupt void epwm2_isr(void);
extern interrupt void epwm3_isr(void);
extern interrupt void epwm4_isr(void);

void main()
{
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    memcpy(&RamfuncsRunStart_1, &RamfuncsLoadStart_1, (size_t)&RamfuncsLoadSize_1);

    InitFlash();

	InitSysCtrl();

	//CLA配置
	CLA_configClaMemory();
	CLA_initCpu1Cla1();

	DINT;

	InitPieCtrl();
	InitFlash();

	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	Pie_Register_Pwm();

	EALLOW;
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM2 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM3 = 1;
    CpuSysRegs.PCLKCR2.bit.EPWM4 = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 1;

    can_init();
    ConfigureADC();

    // 设置 ADCs 转换序列
    SetupADCSoftware();

	//关闭时钟
	EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    //PWM配置
	Init_Pwm();
	Int_Pwm_Enable();

    //开时钟
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    CopyData();

    while(1)
    {
    	can_loop();
        Adc_run();
        LED2_TG;
        DELAY_US(1000 * 500);
    }
}






