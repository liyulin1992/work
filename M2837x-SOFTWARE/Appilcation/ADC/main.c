/**
 * ---------------------------------------------------------------------------------------
 * 			广州创龙电子科技有限公司
 *
 * 			Copyright 2015 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *      本例程会一直采集通道 ADCINA0~ADCINA5, ADCINB0~ADCINB5, ADCIN14, ADCIN15,
 *      ADCINC2~ADCINC5, ADCIND0~ADCIND5的AD数据，并且通过串口A打印。
 * 		电压值计算公式：输入模拟电压 = 采样值 * 3 / 4095.
 *
 * 		XXX 注意：SOC6 将转换 ADCIN14 引脚的输入，SOC7 将转换 ADCIN15 引脚的输入
 *              把 ADCIN14 ADCIN15 结果存入了 AdccResult[0] AdccResult[1]
 * ---------------------------------------------------------------------------------------
 * 时间：2015-12-17
 * ---------------------------------------------------------------------------------------
 */

#include "F28x_Project.h"
#include "SciStdio.h"

void delay_loop(void);
void Gpio_select(void);

void ConfigureADC(void);
void SetupADCSoftware(void);

// 存储ADC结果寄存器
Uint16 AdcaResult[2];
Uint16 AdcbResult[2];

// 存储ADC实际电压值
float Adca_V[2];
float Adcb_V[2];

void main(void)
{
   unsigned int i;

   // 系统初始化，配置PLL，关闭看门狗，使能外设时钟
   InitSysCtrl();

   // GPIO初始化
   InitGpio();

   // GPIO初始化
   Gpio_select();

   // 关中断
   DINT;

   // 初始化Pie控制寄存器
   InitPieCtrl();

   // 不使能中断，清中断标志
   IER = 0x0000;
   IFR = 0x0000;

   // 初始化中断向量表
   InitPieVectTable();

   // 配置 ADCs 及 使能 ADC power
   ConfigureADC();

   // 设置 ADCs 转换序列
   SetupADCSoftware();

   // 初始化串口
   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   SCIPuts("Welcome to TL28377 ADC_SOC Demo application.\r\n\r\n", -1);

   while(1)
   {
	    // 启动 ADCA 转换
		AdcaRegs.ADCSOCFRC1.all = 0x00FF; //XXX SOC0 ~ SOC7
		// 启动 ADCB 转换
		AdcbRegs.ADCSOCFRC1.all = 0x003F; //SOC0 ~ SOC5

		// 等待 ADCA 转换完成
		while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

		// 等待 ADCB 转换完成
		while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

		// 保存 ADCs 结果

		AdcaResult[0] = AdcaResultRegs.ADCRESULT2;
		AdcaResult[1] = AdcaResultRegs.ADCRESULT4;

		AdcbResult[0] = AdcbResultRegs.ADCRESULT2;
		AdcbResult[1] = AdcbResultRegs.ADCRESULT4;

		// 计算 ADCs 的实际电压值
		for(i=0; i<2; i++)
		{
			Adca_V[i] = (float)AdcaResult[i]*3/4095;
			Adcb_V[i] = (float)AdcbResult[i]*3/4095;
		}

		// 串口打印 ADC 各个通道数据
//		for(i=0; i<6; i++)
//		{
//			SCIprintf("ADCINA%d = %d \n",i,AdcaResult[i]);
//		}
//
//		for(i=0; i<6; i++)
//		{
//			SCIprintf("ADCINB%d = %d \n",i,AdcbResult[i]);
//		}

		// 控制 LED1 亮灭
	    GpioDataRegs.GPDTOGGLE.bit.GPIO115 = 1;
	    // 延时 100ms
	    DELAY_US(100000);

   }

}

void ConfigureADC(void)
{
	EALLOW;

	//write configurations
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
	AdcdRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4

	AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

	//Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
	AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	//power up the ADCs
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}

void SetupADCSoftware(void)
{
	Uint16 acqps;

	//采样窗口
	//determine minimum acquisition window (in SYSCLKS) based on resolution
	if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
		acqps = 14; //75ns
	}
	else { //resolution is 16-bit
		acqps = 63; //320ns
	}

	//Select the channels to convert and end of conversion flag
    //ADCA
    EALLOW;

    //SOC0信号转换PIN A1
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    //SOC1信号转换PIN A4
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 4;  //SOC1 will convert pin A1
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

#if 0
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC2 will convert pin A2
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC3 will convert pin A3
    AdcaRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC4 will convert pin A4
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;  //SOC5 will convert pin A5
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCSOC6CTL.bit.CHSEL = 14;  //XXX SOC6 will convert pin ADCIN14
    AdcaRegs.ADCSOC6CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcaRegs.ADCSOC7CTL.bit.CHSEL = 15;  //XXX SOC7 will convert pin ADCIN15
    AdcaRegs.ADCSOC7CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles


    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
#endif

    //ADCB
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert pin B0
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 4;  //SOC1 will convert pin B1
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

#if 0
    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 2;  //SOC2 will convert pin B2
	AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
	AdcbRegs.ADCSOC3CTL.bit.CHSEL = 3;  //SOC3 will convert pin B3
	AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
	AdcbRegs.ADCSOC4CTL.bit.CHSEL = 4;  //SOC4 will convert pin B4
	AdcbRegs.ADCSOC4CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
	AdcbRegs.ADCSOC5CTL.bit.CHSEL = 5;  //SOC5 will convert pin B5
	AdcbRegs.ADCSOC5CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
#endif

}


void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPDMUX2.bit.GPIO115 = 0;	// led1
    GpioCtrlRegs.GPDDIR.bit.GPIO115 = 1;
    GpioCtrlRegs.GPDMUX2.bit.GPIO116 = 0;	// led2
    GpioCtrlRegs.GPDDIR.bit.GPIO116 = 1;

    GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;	// led1
    GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;
    GpioCtrlRegs.GPDMUX2.bit.GPIO112 = 0;	// led2
    GpioCtrlRegs.GPDDIR.bit.GPIO112 = 1;
    GpioCtrlRegs.GPDMUX2.bit.GPIO113 = 0;	// led3
    GpioCtrlRegs.GPDDIR.bit.GPIO113 = 1;
    GpioCtrlRegs.GPDMUX2.bit.GPIO114 = 0;	// led4
    GpioCtrlRegs.GPDDIR.bit.GPIO114 = 1;
    EDIS;

}


