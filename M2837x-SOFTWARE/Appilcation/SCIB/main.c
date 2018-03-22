/**
 * ---------------------------------------------------------------------------------------
 * 			广州创龙电子科技有限公司
 *
 * 			Copyright 2015 Tronlong All rights reserved
 * ---------------------------------------------------------------------------------------
 * 功能描述：
 *     	使用USB转串口模块连接开发板 CON3 TLL UARTA 和 PC 机, 打开串口调试助手，波特率为 115200,
 *     	串口调试助手发送一个字符，DSP 接收到后会回发该字符。
 *     	xxx 请注意一次只能发送一个字符且不能有回车换行，否则会导致串口死机需要断电重启。
 * ---------------------------------------------------------------------------------------
 * 时间：2015-12-17
 * ---------------------------------------------------------------------------------------
 */

#include "F28x_Project.h"
#include "SciStdio.h"

#include "pinmux.h"

// 函数声明
void delay_loop(void);
void Gpio_select(void);
void scib_xmit(int a);
void scib_msg(char * msg);


Uint16 LoopCount;

void main(void)
{
   Uint16 ReceivedChar;
   char *msg;

   // 系统初始化，配置PLL，关闭看门狗，使能外设时钟
   InitSysCtrl();

   // GPIO初始化
   GPIO_setPinMuxConfig();
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

   // 初始化串口
   SCIStdioInit();
   SCIPuts("\r\n ============Test Start===========.\r\n", -1);
   SCIPuts("Welcome to TL28377 SCIB ECHOBACK Demo application.\r\n\r\n", -1);

   msg = "\r\n\n\nHello World!\0";
   scib_msg(msg);

   msg = "\r\nYou will enter a character, and the DSP will echo it back! \n\0";
   scib_msg(msg);

   for(;;)
   {
      msg = "\r\nEnter a character: \0";
      scib_msg(msg);

      // Wait for inc character
      while(ScibRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for XRDY =1 for empty state

      // Get character
      ReceivedChar = ScibRegs.SCIRXBUF.all;

      // Echo character back
      msg = "  You sent: \0";
      scib_msg(msg);
      scib_xmit(ReceivedChar);

      LoopCount++;
   }
}

// Transmit a character from the SCI
void scib_xmit(int a)
{
    while (ScibRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScibRegs.SCITXBUF.all =a;
}

void scib_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scib_xmit(msg[i]);
        i++;
    }
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


