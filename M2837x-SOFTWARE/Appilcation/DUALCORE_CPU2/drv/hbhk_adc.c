/*
 * hbhk_adc.c
 *
 *  Created on: 2017年2月24日
 *      Author: liyulin
 */
#include "hbhk_adc.h"

#include "F28x_Project.h"
// 存储ADC结果寄存器
Uint16 AdcaResult[2];
Uint16 AdcbResult[1];
Uint16 AdccResult[1];

// 存储ADC实际电压值
float Adca_V[2];
float Adcb_V[1];
float Adcc_V[1];

void Adc_run(void)
{
	// 启动 ADC 转换 使能SOC信号
	AdcaRegs.ADCSOCFRC1.all = 0x0007; //XXX SOC0 ~ SOC7
	AdcbRegs.ADCSOCFRC1.all = 0x0007;
	AdccRegs.ADCSOCFRC1.all = 0x00FF;

	// 等待 ADCA 转换完成
	while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

	// 等待 ADCB 转换完成
	while(AdcbRegs.ADCINTFLG.bit.ADCINT1 == 0);
	AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

	// 等待 ADCC 转换完成
	while(AdccRegs.ADCINTFLG.bit.ADCINT1 == 0);
	AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

	// 保存 ADCs 结果
	AdcaResult[0] = AdcaResultRegs.ADCRESULT1;
	AdcaResult[1] = AdcaResultRegs.ADCRESULT2;

	AdcbResult[0] = AdcbResultRegs.ADCRESULT1;

	AdccResult[0] = AdccResultRegs.ADCRESULT6;

	unsigned int i ;
	// 计算 ADCs 的实际电压值
	for(i=0; i<2; i++)
	{
		Adca_V[i] = ( ( (double)AdcaResult[i]*2 /65536- 1) * 3.0 );
	}

	Adcb_V[0] = ( ( (double)AdcbResult[0]*2 /65536- 1) * 3.0 );
	Adcc_V[0] = ( ( (double)AdccResult[0]*2 /65536- 1) * 3.0 );
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

	//ADC_A
    EALLOW;

    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC1 will convert pin A2-A3
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 4;  //SOC2 will convert pin A4-A5
    AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

	//ADC_B
    AdcbRegs.ADCSOC1CTL.bit.CHSEL = 2;  //SOC1 will convert pin B2-B3
    AdcbRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdcbRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcbRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcbRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    //ADC_C
    AdccRegs.ADCSOC6CTL.bit.CHSEL = 14;  //XXX SOC6 will convert pin ADCIN14
    AdccRegs.ADCSOC6CTL.bit.ACQPS = acqps; //sample window is acqps + 1 SYSCLK cycles

    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;
}


