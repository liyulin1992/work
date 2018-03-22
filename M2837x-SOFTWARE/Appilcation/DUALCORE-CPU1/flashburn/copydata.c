#include "F28x_Project.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "flash_programming_c28.h" // Flash API example header file
#include "F021_F2837xD_C28x.h"

Uint16 Reserved[10];
unsigned char ReadReserve[10];

unsigned char key;
unsigned char entrypoint[2];
unsigned char erasedAlready[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Fapi_StatusType oReturnCheck;
Fapi_FlashStatusWordType oFlashStatusWord;

#define BUFFER_SIZE 0x100
Uint16 Buffer[BUFFER_SIZE]; //烧写数据缓存

Uint32 FindSector(Uint32 address);
Uint16 FindSize(Uint32 address);
void CopyData();

unsigned int GetRamData(void);

void ReadReservedFn();

#define NO_ERROR                    0x1000
#define BLANK_ERROR                 0x2000
#define VERIFY_ERROR                0x3000
#define PROGRAM_ERROR               0x4000
#define COMMAND_ERROR               0x5000
#define UNLOCK_ERROR                0x6000

Uint32 longData1 ,longData2; //缓冲变量

typedef struct
{
   Uint16 status;
   Uint32 address;
}  StatusCode;

StatusCode statusCode;

unsigned int *p = (unsigned int *)0x9000;

//失败标记位
int fail = 0;

//文件声明
//FILE *fp;

typedef struct HEADER
 {
     Uint16 BlockSize;
     Uint32 DestAddr;
 } Header;

Header BlockHeader;

unsigned int GetRamData(void)
{
    unsigned int buf;

    buf =(unsigned int)*p++ ;

    return buf;
}

void CopyData()
{
    Uint16 i = 0;
    Uint16 j = 0;
    Uint32 sectorAddress;
    Uint16 sectorSize;
    Uint16 wordData;

    //密钥
    key  = GetRamData();

    ReadReservedFn();

    entrypoint[0] =  GetRamData();
    entrypoint[1] = GetRamData();

//  assert(BUFFER_SIZE % 4 == 0);

    //获取表头数据：快大小
    BlockHeader.BlockSize =GetRamData();

    //当块大小不为0的时候复制数据
    EALLOW;
    while(BlockHeader.BlockSize != (Uint16)0x0000)
    {
//       Fapi_StatusType oReturnCheck;
//       Fapi_FlashStatusWordType oFlashStatusWord;

       //拷贝的目的地址
       //获取高位
       longData1 = GetRamData();
       //低位
       longData2 =  GetRamData();
       BlockHeader.DestAddr = (longData1<<16) | (longData2) ; //拷贝

       for(i = 0; i < BlockHeader.BlockSize; i += 0)
       {
           if(BlockHeader.BlockSize < BUFFER_SIZE)
           {
               for(j = 0; j < BlockHeader.BlockSize; j++)
               {
                   wordData =(Uint16)GetRamData();
                   Buffer[j] = wordData;
                   i++;
               }
               for(j = BlockHeader.BlockSize; j < BUFFER_SIZE; j++)
               {
                   Buffer[j] = 0xFFFF;
               }
           }
           else //BlockHeader.BlockSize >= BUFFER_SIZE
           {
               if((BlockHeader.BlockSize - i) < BUFFER_SIZE)
               {
                   for(j = 0; j < BlockHeader.BlockSize - i; j++)
                   {
                       wordData = (Uint16)GetRamData();
                       Buffer[j] = wordData;
                   }
                   i += j; // increment i outside here so it doesn't affect
                           // loop above
                   for(; j < BUFFER_SIZE; j++) //fill the rest with 0xFFFF
                   {
                       Buffer[j] = 0xFFFF;
                   }
               }
               else
               {
                   for(j = 0; j < BUFFER_SIZE; j++)
                   {
                       wordData = (Uint16)GetRamData();
                       Buffer[j] = wordData;
                       i++;
                   }
               }
           }

           //找地址
           sectorAddress = FindSector(BlockHeader.DestAddr);
           if(sectorAddress != 0xdeadbeef)
           {
               //段大小
               sectorSize = FindSize(sectorAddress);
               //擦数
               oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                                               (uint32 *)sectorAddress);
               //确认是否擦除
               oReturnCheck = Fapi_doBlankCheck((uint32 *)sectorAddress,
                                                  sectorSize,
                                                  &oFlashStatusWord);

               if(oReturnCheck != Fapi_Status_Success)
               {
                    fail++;
               }
           }

           int k;
           Uint16 miniBuffer[4];

           for(k = 0;k < (BUFFER_SIZE / 4); k++)
           {
               miniBuffer[0] = Buffer[k * 4 + 0];
               miniBuffer[1] = Buffer[k * 4 + 1];
               miniBuffer[2] = Buffer[k * 4 + 2];
               miniBuffer[3] = Buffer[k * 4 + 3];

               if(!((miniBuffer[0] == 0xFFFF) && (miniBuffer[1] == 0xFFFF) &&
                       (miniBuffer[2] == 0xFFFF) && (miniBuffer[3] == 0xFFFF)))
               {
                   oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)BlockHeader.DestAddr,
                                                                         miniBuffer,
                                                                         sizeof(miniBuffer),
                                                                         0,
                                                                         0,
                                                                         Fapi_AutoEccGeneration);
               }

               BlockHeader.DestAddr += 0x4;
           }
      }
    //获取下一字块的大小
    BlockHeader.BlockSize = GetRamData();
    }
    EDIS;
    return;
}

Uint32 FindSector(Uint32 address)
{
    if((address >= Bzero_SectorA_start) && (address <= Bzero_SectorA_End) &&
       (erasedAlready[0] == 0))
    {
        erasedAlready[0] = 1;
        return (Uint32)Bzero_SectorA_start;
    }
    else if((address >= Bzero_SectorB_start) &&
            (address <= Bzero_SectorB_End) && (erasedAlready[1] == 0))
    {
        erasedAlready[1] = 1;
        return (Uint32)Bzero_SectorB_start;
    }
    else if((address >= Bzero_SectorC_start) &&
            (address <= Bzero_SectorC_End) && (erasedAlready[2] == 0))
    {
        erasedAlready[2] = 1;
        return (Uint32)Bzero_SectorC_start;
    }
    else if((address >= Bzero_SectorD_start) &&
            (address <= Bzero_SectorD_End) && (erasedAlready[3] == 0))
    {
        erasedAlready[3] = 1;
        return (Uint32)Bzero_SectorD_start;
    }
    else if((address >= Bzero_SectorE_start) &&
            (address <= Bzero_SectorE_End) && (erasedAlready[4] == 0))
    {
        erasedAlready[4] = 1;
        return (Uint32)Bzero_SectorE_start;
    }
    else if((address >= Bzero_SectorF_start) &&
            (address <= Bzero_SectorF_End) && (erasedAlready[5] == 0))
    {
        erasedAlready[5] = 1;
        return (Uint32)Bzero_SectorF_start;
    }
    else if((address >= Bzero_SectorG_start) &&
            (address <= Bzero_SectorG_End) && (erasedAlready[6] == 0))
    {
        erasedAlready[6] = 1;
        return (Uint32)Bzero_SectorG_start;
    }
    else if((address >= Bzero_SectorH_start) &&
            (address <= Bzero_SectorH_End) && (erasedAlready[7] == 0))
    {
        erasedAlready[7] = 1;
        return (Uint32)Bzero_SectorH_start;
    }
    else if((address >= Bzero_SectorI_start) &&
            (address <= Bzero_SectorI_End) && (erasedAlready[8] == 0))
    {
        erasedAlready[8] = 1;
        return (Uint32)Bzero_SectorI_start;
    }
    else if((address >= Bzero_SectorJ_start) &&
            (address <= Bzero_SectorJ_End) && (erasedAlready[9] == 0))
    {
        erasedAlready[9] = 1;
        return (Uint32)Bzero_SectorJ_start;
    }
    else if((address >= Bzero_SectorK_start) &&
            (address <= Bzero_SectorK_End) && (erasedAlready[10] == 0))
    {
        erasedAlready[10] = 1;
        return (Uint32)Bzero_SectorK_start;
    }
    else if((address >= Bzero_SectorL_start) &&
            (address <= Bzero_SectorL_End) && (erasedAlready[11] == 0))
    {
        erasedAlready[11] = 1;
        return (Uint32)Bzero_SectorL_start;
    }
    else if((address >= Bzero_SectorM_start) &&
            (address <= Bzero_SectorM_End) && (erasedAlready[12] == 0))
    {
        erasedAlready[12] = 1;
        return (Uint32)Bzero_SectorM_start;
    }
    else if((address >= Bzero_SectorN_start) &&
            (address <= Bzero_SectorN_End) && (erasedAlready[13] == 0))
    {
        erasedAlready[13] = 1;
        return (Uint32)Bzero_SectorN_start;
    }
    else
    {
        return 0xdeadbeef; // a proxy address to signify that it is not
                           // a flash sector.
    }
}

Uint16 FindSize(Uint32 address)
{
    if(address == Bzero_SectorA_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorB_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorC_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorD_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorE_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorF_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorG_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorH_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorI_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorJ_start)
    {
        return Bzero_64KSector_u32length;
    }
    else if(address == Bzero_SectorK_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorL_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorM_start)
    {
        return Bzero_16KSector_u32length;
    }
    else if(address == Bzero_SectorN_start)
    {
        return Bzero_16KSector_u32length;
    }

    // no other possible case
    return 0xbeef;
}

void ReadReservedFn()
{
    Uint16 i;

    // Read and discard the 8 reserved words.
    for(i = 1; i <= 8; i++)
    {
       GetRamData();
    }

    return;
}
