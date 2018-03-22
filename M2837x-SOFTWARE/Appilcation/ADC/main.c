/**
 * ---------------------------------------------------------------------------------------
 * 			���ݴ������ӿƼ����޹�˾
 *
 * 			Copyright 2015 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      �����̻�һֱ�ɼ�ͨ�� ADCINA0~ADCINA5, ADCINB0~ADCINB5, ADCIN14, ADCIN15,
 *      ADCINC2~ADCINC5, ADCIND0~ADCIND5��AD���ݣ�����ͨ������A��ӡ��
 * 		��ѹֵ���㹫ʽ������ģ���ѹ = ����ֵ * 3 / 4095.
 *
 * 		XXX ע�⣺SOC6 ��ת�� ADCIN14 ���ŵ����룬SOC7 ��ת�� ADCIN15 ���ŵ�����
 *              �� ADCIN14 ADCIN15 ��������� AdccResult[0] AdccResult[1]
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2015-12-17
 * ---------------------------------------------------------------------------------------
 */

#include "F28x_Project.h"
#include "SciStdio.h"

void delay_loop(void);
void Gpio_select(void);

void ConfigureADC(void);
void SetupADCSoftware(void);

// �洢ADC����Ĵ���
Uint16 AdcaResult[2];
Uint16 AdcbResult[2];

// �洢ADCʵ�ʵ�ѹֵ
float Adca_V[2];
float Adcb_V[2];

void main(void)
{
   unsigned int i;

   // ϵͳ��ʼ��������PLL���رտ��Ź���ʹ������ʱ��
   InitSysCtrl();

   // GPIO��ʼ��
   InitGpio();

   // GPIO��ʼ��
   Gpio_select();

   // ���ж�
   DINT;

   // ��ʼ��Pie���ƼĴ���
   InitPieCtrl();

   // ��ʹ���жϣ����жϱ�־
   IER = 0x0000;
   IFR = 0x0000;

   // ��ʼ���ж�������
   InitPieVectTable();

   // ���� ADCs �� ʹ�� ADC power
   ConfigureADC();

   // ���� ADCs ת������
   SetupADCSoftware();

   // ��ʼ������
   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   SCIPuts("Welcome to TL28377 ADC_SOC Demo application.\r\n\r\n", -1);

   while(1)
   {
	    // ���� ADCA ת��
		AdcaRegs.ADCSOCFRC1.all = 0x00FF; //XXX SOC0 ~ SOC7
		// ���� ADCB ת��
		AdcbRegs.ADCSOCFRC1.all = 0x003F; //SOC0 ~ SOC5

		// �ȴ� ADCA ת�����
		while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

		// �ȴ� ADCB ת�����
		while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

		// ���� ADCs ���

		AdcaResult[0] = AdcaResultRegs.ADCRESULT2;
		AdcaResult[1] = AdcaResultRegs.ADCRESULT4;

		AdcbResult[0] = AdcbResultRegs.ADCRESULT2;
		AdcbResult[1] = AdcbResultRegs.ADCRESULT4;

		// ���� ADCs ��ʵ�ʵ�ѹֵ
		for(i=0; i<2; i++)
		{
			Adca_V[i] = (float)AdcaResult[i]*3/4095;
			Adcb_V[i] = (float)AdcbResult[i]*3/4095;
		}

		// ���ڴ�ӡ ADC ����ͨ������
//		for(i=0; i<6; i++)
//		{
//			SCIprintf("ADCINA%d = %d \n",i,AdcaResult[i]);
//		}
//
//		for(i=0; i<6; i++)
//		{
//			SCIprintf("ADCINB%d = %d \n",i,AdcbResult[i]);
//		}

		// ���� LED1 ����
	    GpioDataRegs.GPDTOGGLE.bit.GPIO115 = 1;
	    // ��ʱ 100ms
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

	//��������
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

    //SOC0�ź�ת��PIN A1
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  //SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    //SOC1�ź�ת��PIN A4
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


