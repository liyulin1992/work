/**
 * ---------------------------------------------------------------------------------------
 * 			���ݴ������ӿƼ����޹�˾
 *
 * 			Copyright 2015 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * ����������
 *      ʹ��Micro USB�����ӿ�����USB TO UARTB��PC��, �򿪴��ڵ�������, ������Ϊ 115200,
 *     	���ڵ������ַ���һ���ַ���DSP ���յ����ط����ַ���
 *     	xxx ��ע��һ��ֻ�ܷ���һ���ַ��Ҳ����лس����У�����ᵼ�´���������Ҫ�ϵ�������
 * ---------------------------------------------------------------------------------------
 * ʱ�䣺2015-12-17
 * ---------------------------------------------------------------------------------------
 */
#include "F28x_Project.h"
#include "SciStdio.h"
#include "pinmux.h"

// ��������
void delay_loop(void);
//void Gpio_select(void);
void scic_xmit(int a);
void scic_msg(char * msg);

Uint16 LoopCount;

void main(void)
{
   Uint16 ReceivedChar;
   char *msg;

   // ϵͳ��ʼ��������PLL���رտ��Ź���ʹ������ʱ��
   InitSysCtrl();

   // GPIO��ʼ��
   GPIO_setPinMuxConfig();

//   GPIO_SetupPinOptions(139, GPIO_INPUT, GPIO_ASYNC);
//   GPIO_SetupPinOptions(140, GPIO_OUTPUT, GPIO_PUSHPULL);

   // ���ж�
   DINT;

   // ��ʼ��Pie���ƼĴ���
   InitPieCtrl();

   // ��ʹ���жϣ����жϱ�־
   IER = 0x0000;
   IFR = 0x0000;

   // ��ʼ���ж�������
   InitPieVectTable();

   // ��ʼ������
   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   SCIPuts("Welcome to TL28377 Scic ECHOBACK Demo application.\r\n\r\n", -1);

   msg = "\r\n\n\nHello World!\0";
   scic_msg(msg);

   msg = "\r\nYou will enter a character, and the DSP will echo it back! \n\0";

   scic_msg(msg);

   for(;;)
   {
      msg = "\r\nEnter a character: \0";
      scic_msg(msg);

      // Wait for inc character
      while(ScicRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for XRDY =1 for empty state

      // Get character
      ReceivedChar = ScicRegs.SCIRXBUF.all;

      // Echo character back
      msg = "  You sent: \0";
      scic_msg(msg);
      scic_xmit(ReceivedChar);

      LoopCount++;
   }
}

// Transmit a character from the SCI
void scic_xmit(int a)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF.all =a;
}

void scic_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scic_xmit(msg[i]);
        i++;
    }
}

//void Gpio_select(void)
//{
//    EALLOW;
//    GpioCtrlRegs.GPDMUX2.bit.GPIO115 = 0;	// led1
//    GpioCtrlRegs.GPDDIR.bit.GPIO115 = 1;
//    GpioCtrlRegs.GPDMUX2.bit.GPIO116 = 0;	// led2
//    GpioCtrlRegs.GPDDIR.bit.GPIO116 = 1;
//
//    GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;	// led1
//    GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;
//    GpioCtrlRegs.GPDMUX2.bit.GPIO112 = 0;	// led2
//    GpioCtrlRegs.GPDDIR.bit.GPIO112 = 1;
//    GpioCtrlRegs.GPDMUX2.bit.GPIO113 = 0;	// led3
//    GpioCtrlRegs.GPDDIR.bit.GPIO113 = 1;
//    GpioCtrlRegs.GPDMUX2.bit.GPIO114 = 0;	// led4
//    GpioCtrlRegs.GPDDIR.bit.GPIO114 = 1;
//    EDIS;
//
//}
//===========================================================================
// No more.
//===========================================================================

