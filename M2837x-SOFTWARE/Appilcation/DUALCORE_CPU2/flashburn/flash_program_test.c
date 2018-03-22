#include "F28x_Project.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "flash_programming_c28.h" // Flash API example header file
#include "F021_F2837xD_C28x.h"

unsigned char buffer_convert_1[30];      //转换前

void Init_Flash_Sectors(void);

extern void ReadReservedFn();
extern void CopyData();
extern unsigned int GetWordData(void);

//extern Uint16 RamfuncsLoadStart;
//extern Uint16 RamfuncsLoadSize;
//extern Uint16 RamfuncsRunStart;

extern unsigned long GetRamData(void);

void flash_burn(void)
{
//    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);

    InitSysCtrl();

    InitGpio();

    DINT;

    IER = 0x0000;
    IFR = 0x0000;

    //初始化FLASH
    InitFlash();

    //获得PUMP信号量
    SeizeFlashPump();

    //初始化段
    Init_Flash_Sectors();

    //复制数据到FLASH
    CopyData();

//    while(1);
}

// Init_Flash_Sectors - Initialize flash API and active flash bank sectors
void Init_Flash_Sectors(void)
{
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;

    Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 150);

    Fapi_setActiveFlashBank(Fapi_FlashBank0);
    Fapi_setActiveFlashBank(Fapi_FlashBank1);
    Fapi_setActiveFlashBank(Fapi_FlashBank2);
    Fapi_setActiveFlashBank(Fapi_FlashBank3);
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;
    EDIS;
}


