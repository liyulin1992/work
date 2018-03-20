#include "device.h"
#include "driverlib.h"

#define MCBSP_CYCLE_NOP0(n)  __asm(" RPT #(" #n ") || NOP")
#define MCBSP_CYCLE_NOP(n)   MCBSP_CYCLE_NOP0(n)

//#define WORD_SIZE     8U
#define WORD_SIZE     16U
//#define WORD_SIZE     32U

uint32_t errCountGlobal   = 0;

uint16_t txData1 = 0x0000;
uint16_t txData2 = 0x0000;
uint16_t rxData1 = 0x0000;
uint16_t rxData2 = 0x0000;
uint16_t dataSize;

//函数声明
extern void setupMcBSPAPinmux(void);
void initMcBSPLoopback(void);

void main(void)
{
    uint32_t tempData;
    errCountGlobal = 0x0;

    //设备初始化
    Device_init();

    //禁止中断
    DINT;

    //GPIO初始化
    Device_initGPIO();

    //配置MCBSP管脚
    setupMcBSPAPinmux();

    //初始化中断
    Interrupt_initModule();

    //初始化PIE
    Interrupt_initVectorTable();
    dataSize = WORD_SIZE;

    //初始化MCBSP
    initMcBSPLoopback();

    //回环测试
    if(dataSize == 8)
    {
        txData1 = 0x0000;
        while(1)
        {
            //发送数据
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write16bitData(MCBSPA_BASE, txData1);

            //接受数据
            while(!McBSP_isRxReady(MCBSPA_BASE));
            rxData1 = McBSP_read16bitData(MCBSPA_BASE);

            //校验
            if(rxData1 != txData1)
            {
                errCountGlobal++;
                ESTOP0;
            }
			
            txData1++;
            txData1 = txData1 & 0x00FF;
            NOP;
        }
    }

    else if(dataSize == 16)
    {
        txData1 = 0x0000;
        while(1)
        {
            //发送数据
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write16bitData(MCBSPA_BASE, txData1);

            //接受数据
            while(!McBSP_isRxReady(MCBSPA_BASE));
            rxData1 = McBSP_read16bitData(MCBSPA_BASE);
            if(rxData1 != txData1)
            {
                errCountGlobal++;
                ESTOP0;
            }
            txData1++;
            NOP;
        }
    }

    else if(dataSize == 32)
    {
        txData2 = 0xFFFF;
        txData1 = 0x0000;
        while(1)
        {
            //发送数据
            tempData = (txData1|(((uint32_t)txData2) << 16U));
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write32bitData(MCBSPA_BASE, tempData);

            //接受数据
            while(!McBSP_isRxReady(MCBSPA_BASE));
            tempData = McBSP_read32bitData(MCBSPA_BASE);
            rxData1 = tempData & 0xFFFF;
            rxData2 = tempData >> 16U;
            if(rxData1 != txData1 || rxData2 != txData2)
            {
                errCountGlobal++;
                ESTOP0;
            }
            txData1++;
            txData2--;
            NOP;
        }
    }
}

//初始化配置
void initMcBSPLoopback()
{
    //复位
    McBSP_resetFrameSyncLogic(MCBSPA_BASE);
    McBSP_resetSampleRateGenerator(MCBSPA_BASE);
    McBSP_resetTransmitter(MCBSPA_BASE);
    McBSP_resetReceiver(MCBSPA_BASE);

    //符号扩展和对齐模式
    McBSP_setRxSignExtension(MCBSPA_BASE, MCBSP_RIGHT_JUSTIFY_FILL_ZERO);

    //使能回环模式
    McBSP_enableLoopback(MCBSPA_BASE);

    //数据延迟位
    McBSP_setRxDataDelayBits(MCBSPA_BASE, MCBSP_DATA_DELAY_BIT_1);
    McBSP_setTxDataDelayBits(MCBSPA_BASE, MCBSP_DATA_DELAY_BIT_1);

    //时钟源选择
    McBSP_setTxClockSource(MCBSPA_BASE, MCBSP_INTERNAL_TX_CLOCK_SOURCE);
    McBSP_setTxFrameSyncSource(MCBSPA_BASE, MCBSP_TX_INTERNAL_FRAME_SYNC_SOURCE);

    //数据大小配置
    if(dataSize == 8)
    {
        McBSP_setRxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_8, 0);
        McBSP_setTxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_8, 0);
    }

    else if(dataSize == 16)
    {
        McBSP_setRxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_16, 0);
        McBSP_setTxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_16, 0);
    }
    else if(dataSize == 32)
    {
        McBSP_setRxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_32, 0);
        McBSP_setTxDataSize(MCBSPA_BASE, MCBSP_PHASE_ONE_FRAME,
                            MCBSP_BITS_PER_WORD_32, 0);
    }

    //帧同步脉冲周期
    McBSP_setFrameSyncPulsePeriod(MCBSPA_BASE, 320);

    //帧同步脉冲宽度
    McBSP_setFrameSyncPulseWidthDivider(MCBSPA_BASE, 1);

    //帧同步脉冲触发源.
    McBSP_setTxInternalFrameSyncSource(MCBSPA_BASE,
                                       MCBSP_TX_INTERNAL_FRAME_SYNC_SRG);

    //采样率发生器
    McBSP_setTxSRGClockSource(MCBSPA_BASE, MCBSP_SRG_TX_CLOCK_SOURCE_LSPCLK);

    //时钟分频系数
    McBSP_setSRGDataClockDivider(MCBSPA_BASE, 1);

    //禁止外部时钟同步
    McBSP_disableSRGSyncFSR(MCBSPA_BASE);

    //延迟
    MCBSP_CYCLE_NOP(8);

    //使能时钟发生器和帧同步逻辑单元
    McBSP_enableSampleRateGenerator(MCBSPA_BASE);
    McBSP_enableFrameSyncLogic(MCBSPA_BASE);
    MCBSP_CYCLE_NOP(16);

    //解除复位
    McBSP_enableTransmitter(MCBSPA_BASE);
    McBSP_enableReceiver(MCBSPA_BASE);
}
