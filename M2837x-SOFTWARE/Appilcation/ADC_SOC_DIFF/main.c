#include "F28x_Project.h"
#include "SciStdio.h"
//#include <stdio.h>


void delay_loop(void);
void Gpio_select(void);

void ConfigureADC(void);
void SetupADCSoftware(void);

// �洢ADC����Ĵ���
Uint16 AdcaResult[2];
Uint16 AdcbResult[1];
Uint16 AdccResult[1];

// �洢ADCʵ�ʵ�ѹֵ
float Adca_V[2];
float Adcb_V[1];
float Adcc_V[1];

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
	    // ���� ADC ת�� ʹ��SOC�ź�
		AdcaRegs.ADCSOCFRC1.all = 0x0007; //XXX SOC0 ~ SOC7
		AdcbRegs.ADCSOCFRC1.all = 0x0007;
		AdccRegs.ADCSOCFRC1.all = 0x00FF;

		// �ȴ� ADCA ת�����
		while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
		
		// �ȴ� ADCB ת�����
		while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
		
		// �ȴ� ADCC ת�����
		while(AdccRegs.ADCINTFLG.bit.ADCINT1 == 0);
		AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

		// ���� ADCs ���
		AdcaResult[0] = AdcaResultRegs.ADCRESULT1;
		AdcaResult[1] = AdcaResultRegs.ADCRESULT2;

		AdcbResult[0] = AdcbResultRegs.ADCRESULT1;
//		AdcbResult[1] = AdcbResultRegs.ADCRESULT2;

		AdccResult[0] = AdccResultRegs.ADCRESULT6;

		// ���� ADCs ��ʵ�ʵ�ѹֵ
		for(i=0; i<2; i++)
		{
			Adca_V[i] = ( ( (double)AdcaResult[i]*2 /65536- 1) * 3.0 );

		}

		Adcb_V[0] = ( ( (double)AdcbResult[0]*2 /65536- 1) * 3.0 );
		Adcc_V[0] = ( ( (double)AdccResult[0]*2 /65536- 1) * 3.0 );
		
		// ���ڴ�ӡ ADC ����ͨ������
		for(i=0; i<2; i++)
		{
			SCIprintf("ADCINA%d = %d \n",i,AdcaResult[i]);
			SCIprintf("ADCINB%d = %d \n",i,AdcbResult[i]);
		}

		SCIprintf("ADCINC1 = %d \n",AdccResult[0]);
		
		/*����ע�͵�*/
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
	AdcbRegs.ADCCTL2.bit.PRESCALE = 6;
	AdccRegs.ADCCTL2.bit.PRESCALE = 6;

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);
    AdcSetMode(ADC_ADCB, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);
    AdcSetMode(ADC_ADCC, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);

    //Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
	AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1;
	AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	//power up the ADCs
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1;
	AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}

void SetupADCSoftware(void)
{
	Uint16 acqps;

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
//    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0-A1
//    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC1 will convert pin A2-A3
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC2 will convert pin A4-A5
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

//    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin B0-B1
//    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC1 will convert pin B2-B3
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

//    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC2 will convert pin B4-B5
//    AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    /*ADC_C*/
    AdccRegs.ADCSOC6CTL.bit.CHSEL = 14;  //XXX SOC6 will convert pin ADCIN14
    AdccRegs.ADCSOC6CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles
//    AdccRegs.ADCSOC7CTL.bit.CHSEL = 15;  //XXX SOC7 will convert pin ADCIN15
//    AdccRegs.ADCSOC7CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;
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