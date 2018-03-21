#include "device.h"
#include "driverlib.h"
#include <stdio.h>

//CS片选地址
#define ASRAM_CS3_START_ADDR 0x300000           // EMIF1 CS3 地址
//数据长度
#define ASRAM_CS3_SIZE 0x1000

//时序参数配置结构体
EMIF_AsyncTimingParams tparam;

//数组
uint32_t w_data_b[ASRAM_CS3_SIZE];
uint32_t r_data_b[ASRAM_CS3_SIZE];

//
uint16_t Err_Check;

//函数声明
void Emif_Init();
extern void setupEMIF1PinmuxAsync16Bit(void);

uint16_t Read_RAM(uint32_t startAddr, uint32_t memSize,uint32_t *data_r);
uint16_t Write_RAM(uint32_t startAddr, uint32_t memSize,uint32_t *data_w);

void main()
{
	Device_init();

    DINT;

    Device_initGPIO();

    Interrupt_initModule();

    Interrupt_initVectorTable();
	
	Emif_Init();
	
	uint16_t j;
	for(j=0;j<ASRAM_CS3_SIZE;j++)
	{
	    w_data_b[j] = 0x1234;
	}

	Write_RAM(ASRAM_CS3_START_ADDR,ASRAM_CS3_SIZE,w_data_b);
	Read_RAM(ASRAM_CS3_START_ADDR,ASRAM_CS3_SIZE,r_data_b);

	//校验
	for(j=0;j<ASRAM_CS3_SIZE;j++)
	{
	    if(w_data_b[j]!=r_data_b[j])
	    {
	        Err_Check++;
	    }
	}

	while(1);
}

void Emif_Init()
{
	//EMIF时钟
    SysCtl_setEMIF1ClockDivider(SYSCTL_EMIF1CLK_DIV_1);

    //EMIF1控制权
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_G);

    //
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);

    //配置EMIF管脚
    setupEMIF1PinmuxAsync16Bit();

    //异步模式
    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS3_OFFSET,
                      EMIF_ASYNC_NORMAL_MODE);

    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS3_OFFSET);

    //数据宽度
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS3_OFFSET,
                              EMIF_ASYNC_DATA_WIDTH_16);
							   // Configure the access timing for CS2 space.
    //时序配置
    tparam.rSetup = 2;
    tparam.rStrobe =8;
    tparam.rHold = 2;
    tparam.turnArnd = 1;
    tparam.wSetup = 2;
    tparam.wStrobe = 2;
    tparam.wHold = 2;
    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS3_OFFSET, &tparam);
}

uint16_t Read_RAM(uint32_t startAddr, uint32_t memSize,uint32_t *data_r)
{
    uint16_t *memPtr;
    uint32_t i;
	
    memPtr = (uint16_t *)startAddr;
	
    for(i = 0; i < memSize; i++)
    {
        *data_r++ = *memPtr++; 
    }
	
	return 1;
}

uint16_t Write_RAM(uint32_t startAddr, uint32_t memSize,uint32_t *data_w)
{
    uint16_t *memPtr;
    uint32_t i;	

    memPtr = (uint16_t*)startAddr;

    for(i=0; i < memSize ; i ++)
    {
        *memPtr++ = *data_w++;
    }

    return 1;
}
