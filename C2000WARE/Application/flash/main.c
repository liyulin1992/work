#include "device.h"
#include "driverlib.h"

//CS片选地址
#define FLASH_CS2_START_ADDR 0x100000
//数据长度
#define FLASH_CS2_SIZE 0x1000

//时序参数配置结构体
EMIF_AsyncTimingParams tparam;

//数组
uint32_t w_data_b[FLASH_CS2_SIZE];
uint32_t r_data_b[FLASH_CS2_SIZE];

volatile uint32_t *FlashStart = (volatile uint32_t*)FLASH_CS2_START_ADDR;

//函数声明
extern void setupEMIF1PinmuxAsync16Bit(void);

void FlashReset(void);
void Emif_Init();

int16_t FlashReadId(void);
int16_t ChipErase(void);

uint16_t Read_Flash(uint32_t startAddr, uint32_t memSize,uint32_t *data_r);
uint16_t Write_Flash(uint32_t startAddr, uint32_t memSize,uint32_t *data_w);

void main()
{
	Device_init();

    DINT;

    Device_initGPIO();

    Interrupt_initModule();

    Interrupt_initVectorTable();
	
	Emif_Init();
	
	while(!FlashReadId());

	//擦除
	while(!ChipErase());

	uint16_t j;
	for(j=0;j<FLASH_CS2_SIZE;j++)
	{
	    w_data_b[j] = 0x1234;
	}

	//写
	Write_Flash(FLASH_CS2_START_ADDR,FLASH_CS2_SIZE,w_data_b);

	while(1);
}

void Emif_Init()
{
	//EMIF时钟
    SysCtl_setEMIF1ClockDivider(SYSCTL_EMIF1CLK_DIV_1);

    //EMIF1控制权
    EMIF_selectMaster(EMIF1CONFIG_BASE, EMIF_MASTER_CPU1_NG);

    //
    EMIF_setAccessProtection(EMIF1CONFIG_BASE, 0x0);

    EMIF_commitAccessConfig(EMIF1CONFIG_BASE);

    EMIF_lockAccessConfig(EMIF1CONFIG_BASE);

    //配置EMIF管脚
    setupEMIF1PinmuxAsync16Bit();

    //异步模式
    EMIF_setAsyncMode(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET,
                      EMIF_ASYNC_NORMAL_MODE);

    EMIF_disableAsyncExtendedWait(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET);

    //数据宽度
    EMIF_setAsyncDataBusWidth(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET,
                              EMIF_ASYNC_DATA_WIDTH_16);
							   // Configure the access timing for CS2 space.
    //时序配置
    tparam.rSetup = 1;
    tparam.rStrobe = 15;
    tparam.rHold = 1;
    tparam.turnArnd = 0;
    tparam.wSetup = 1;
    tparam.wStrobe = 15;
    tparam.wHold = 1;
    EMIF_setAsyncTimingParams(EMIF1_BASE, EMIF_ASYNC_CS2_OFFSET, &tparam);
}

void FlashReset(void)
{
    *(FlashStart + 0x5555) =0xaa;
    *(FlashStart + 0x2AAA) =0x55;
    *(FlashStart + 0x5555) =0xf0;
    DEVICE_DELAY_US(100);
}

int16_t FlashReadId(void)
{
    uint16_t MakerId,DeviceId;
    uint16_t Active;
    *(FlashStart + 0x05555) = 0xaa;
    *(FlashStart + 0x02AAA) = 0x55;
    *(FlashStart + 0x05555) = 0x90;

//    MakerId = *(FlashStart + 0x0000) & 0xFF;
//    DeviceId = *(FlashStart + 0x0001) & 0xFF;
    MakerId = *(FlashStart + 0x0000);
    DeviceId = *(FlashStart + 0x0001);

    //SST39VF800A MakerId=0xBF DeviceId=0x81
    Active = (MakerId == 0xBF) && (DeviceId == 0x81);
    FlashReset();
    return Active;
}

int16_t ChipErase(void)
{
    uint16_t  i,Data;
    uint32_t  TimeOut;
    *(FlashStart + 0x5555) = 0xAAAA;
    *(FlashStart + 0x2AAA) = 0x5555;
    *(FlashStart + 0x5555) = 0x8080;
    *(FlashStart + 0x5555) = 0xAAAA;
    *(FlashStart + 0x2AAA) = 0x5555;
    *(FlashStart + 0x5555) = 0x1010;

    i = 0;
    TimeOut = 0;

    DEVICE_DELAY_US(15000);

    while(i<5)
    {
        Data = *(FlashStart +  0x3FFFF);
        if  (Data == 0xFFFF)    i++;
        else    i=0;
        if ( ++TimeOut>0x1000000)   return (-2);
    }

    for (i=0;i<0x4000;i++)
    {
         Data = *(FlashStart + i);
         if (Data !=0xFFFF)  return (-1);
    }

   return  (1);
}

uint16_t Read_Flash(uint32_t startAddr, uint32_t memSize,uint32_t *data_r)
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

uint16_t Write_Flash(uint32_t startAddr, uint32_t memSize,uint32_t *data_w)
{
    uint32_t *memPtr;
    uint32_t i;

    memPtr = (uint32_t*)startAddr;

    for(i=0; i < memSize ; i ++)
    {
        //写命令
        *(FlashStart + 0x5555)= 0xAA;
        *(FlashStart + 0x2AAA)= 0x55;
        *(FlashStart + 0x5555) = 0xA0;

        *memPtr++ = *data_w++;
        DEVICE_DELAY_US(50);
    }

    return 1;
}
