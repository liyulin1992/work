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

//��������
extern void setupMcBSPAPinmux(void);
void initMcBSPLoopback(void);

void main(void)
{
    uint32_t tempData;
    errCountGlobal = 0x0;

    //�豸��ʼ��
    Device_init();

    //��ֹ�ж�
    DINT;

    //GPIO��ʼ��
    Device_initGPIO();

    //����MCBSP�ܽ�
    setupMcBSPAPinmux();

    //��ʼ���ж�
    Interrupt_initModule();

    //��ʼ��PIE
    Interrupt_initVectorTable();
    dataSize = WORD_SIZE;

    //��ʼ��MCBSP
    initMcBSPLoopback();

    //�ػ�����
    if(dataSize == 8)
    {
        txData1 = 0x0000;
        while(1)
        {
            //��������
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write16bitData(MCBSPA_BASE, txData1);

            //��������
            while(!McBSP_isRxReady(MCBSPA_BASE));
            rxData1 = McBSP_read16bitData(MCBSPA_BASE);

            //У��
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
            //��������
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write16bitData(MCBSPA_BASE, txData1);

            //��������
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
            //��������
            tempData = (txData1|(((uint32_t)txData2) << 16U));
            while(!McBSP_isTxReady(MCBSPA_BASE));
            McBSP_write32bitData(MCBSPA_BASE, tempData);

            //��������
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

//��ʼ������
void initMcBSPLoopback()
{
    //��λ
    McBSP_resetFrameSyncLogic(MCBSPA_BASE);
    McBSP_resetSampleRateGenerator(MCBSPA_BASE);
    McBSP_resetTransmitter(MCBSPA_BASE);
    McBSP_resetReceiver(MCBSPA_BASE);

    //������չ�Ͷ���ģʽ
    McBSP_setRxSignExtension(MCBSPA_BASE, MCBSP_RIGHT_JUSTIFY_FILL_ZERO);

    //ʹ�ܻػ�ģʽ
    McBSP_enableLoopback(MCBSPA_BASE);

    //�����ӳ�λ
    McBSP_setRxDataDelayBits(MCBSPA_BASE, MCBSP_DATA_DELAY_BIT_1);
    McBSP_setTxDataDelayBits(MCBSPA_BASE, MCBSP_DATA_DELAY_BIT_1);

    //ʱ��Դѡ��
    McBSP_setTxClockSource(MCBSPA_BASE, MCBSP_INTERNAL_TX_CLOCK_SOURCE);
    McBSP_setTxFrameSyncSource(MCBSPA_BASE, MCBSP_TX_INTERNAL_FRAME_SYNC_SOURCE);

    //���ݴ�С����
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

    //֡ͬ����������
    McBSP_setFrameSyncPulsePeriod(MCBSPA_BASE, 320);

    //֡ͬ��������
    McBSP_setFrameSyncPulseWidthDivider(MCBSPA_BASE, 1);

    //֡ͬ�����崥��Դ.
    McBSP_setTxInternalFrameSyncSource(MCBSPA_BASE,
                                       MCBSP_TX_INTERNAL_FRAME_SYNC_SRG);

    //�����ʷ�����
    McBSP_setTxSRGClockSource(MCBSPA_BASE, MCBSP_SRG_TX_CLOCK_SOURCE_LSPCLK);

    //ʱ�ӷ�Ƶϵ��
    McBSP_setSRGDataClockDivider(MCBSPA_BASE, 1);

    //��ֹ�ⲿʱ��ͬ��
    McBSP_disableSRGSyncFSR(MCBSPA_BASE);

    //�ӳ�
    MCBSP_CYCLE_NOP(8);

    //ʹ��ʱ�ӷ�������֡ͬ���߼���Ԫ
    McBSP_enableSampleRateGenerator(MCBSPA_BASE);
    McBSP_enableFrameSyncLogic(MCBSPA_BASE);
    MCBSP_CYCLE_NOP(16);

    //�����λ
    McBSP_enableTransmitter(MCBSPA_BASE);
    McBSP_enableReceiver(MCBSPA_BASE);
}
