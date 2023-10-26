/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *       Author:  Mahmoud Badr
 *         File:  Bootloader.c
 *        Layer:  App
 *       Module:  Bootloader
 *      Version:  1.00
 *
 *  Description:  -
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include "Uart.h"
#include "Bootloader.h"
#include "driverlib/flash.h"
#include "driverlib/can.h"

/**********************************************************************************************************************
 *  LOCAL DATA
 *********************************************************************************************************************/
static uint8_t BL_HostBuffer[BL_HOST_BUFFER_SIZE]={0};
static uint8_t BL_Command=0U;
static uint32_t BL_AppAddress=0U;
static uint8_t BL_AddFlag=0U;
static void(*APP_ResetHandler)(void)=NULL;
static void(*BL_FuncPtrArr[])(void)={BL_GetVersion,BL_GetHelp,BL_GetChipID,BL_ReadProtectLevel,BL_GoToAdd,BL_EraseFlash,BL_WriteMem,
                                     BL_SetWriteProtoect,BL_ReadMem,BL_GetWriteProtoectState,BL_ReadOTP,BL_SetProtectLevel,BL_JUmpToUserAppCmd};

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : void BL_PrintMesssage(char *Copy_Format,...)
 * \Description     : Variatic Function used to send Messages to debug port
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_Format: The Format of Sent Data
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_PrintMesssage(char *Copy_Format,...)
{
    char Local_Message[100]={0};
    va_list List;
    /*Enables Access to variable Arguments*/
    va_start(List, Copy_Format);
    /*Write Formatted Data from from variable argument list to string*/
    vsprintf(Local_Message,Copy_Format,List);
#if BL_DEBUG_METHODE==BL_UART_DEBUG
    /*Transmit the formatted data throw Your Method*/
    UART_SendBytes(BL_DEBUG_UART, (uint8_t*)Local_Message, sizeof(Local_Message));
#elif BL_DEBUG_METHODE==BL_CAN_DUBUG

#endif
    /*Performs cleanup for an ap object initialized by a call to va_start*/
    va_end(List);
}

/************************************************************************************************
 * \Syntax          : void BL_SendDataToHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen)
 * \Description     : Send Data to Host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_HostBuffer: Buffer sent to Host
 *                    Copy_DataLen: the Length of Buffer being sent to Host
 * \Parameters (out): None
 * \Return value:   : void
 ************************************************************************************************/
static void BL_SendDataToHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen)
{
    /*Choose the Communication Protocol Between bootloader and Host*/
#if BL_COMM_PROTOCOL==BL_UART_COMM
    UART_SendBytes(BL_COMM_METHODE, Copy_HostBuffer, Copy_DataLen);
#elif BL_COMM_PROTOCOL==BL_CAN_COMM

#endif
}

/************************************************************************************************
 * \Syntax          : void BL_ReceiveDataFromHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen)
 * \Description     : Send Data to Host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_HostBuffer: Buffer Received from Host
 *                    Copy_DataLen: the Length of Buffer being Received from Host
 * \Parameters (out): None
 * \Return value:   : void
 ************************************************************************************************/
static void BL_ReceiveDataFromHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen)
{
    /*Choose the Communication Protocol Between bootloader and Host*/
#if BL_COMM_PROTOCOL==BL_UART_COMM
    UART_ReceiveBytes(BL_COMM_METHODE, Copy_HostBuffer, Copy_DataLen);
#elif BL_COMM_PROTOCOL==BL_CAN_COMM

#endif
}

/********************************************************************************************************
 * \Syntax          : bool BL_PerformFlashErase(uint8_t Copy_FirstSector,uint8_t Copy_NumofSectors)
 * \Description     : Erase Number of Sectors from Flash memory
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_FirstSector:  The sector start erasing from
 *                    Copy_NumofSectors: Number of Sectors to be Erased
 * \Parameters (out): None
 * \Return value:   : bool
 *                    true - false
 **********************************************************************************************************/
static bool BL_PerformFlashErase(uint8_t Copy_FirstSector,uint8_t Copy_NumofSectors)
{
    bool Local_EraseState=false;
    uint32_t Local_SectorStart=0;
    uint32_t Local_SectorState=0;
    uint8_t Local_Counter=0;
    /*Make Sure that the user wants to Erase total number of sector <= Available Sectors*/
    if((BL_FLASH_SECTORS_NUM > (uint8_t)(Copy_NumofSectors-Copy_FirstSector)) || (Copy_FirstSector == BL_MASS_ERASE))
    {
        Local_EraseState=true;
        /*Check if the user wants a mass Erase*/
        if(Copy_FirstSector == BL_MASS_ERASE)
        {
            Local_Counter=0;
            Copy_NumofSectors=255;
            Copy_FirstSector=0;
        }
        else
        {
            Local_Counter=Copy_FirstSector;
        }
        for( ;Local_Counter<(Copy_NumofSectors+Copy_FirstSector);Local_Counter++)
        {
            /*Calculate the Sector Start Address*/
            Local_SectorStart=FLASH_START_ADDRESS+(Local_Counter*FLASH_SECTOR_SIZE);
            /*Erase the sectors*/
            Local_SectorState=FlashErase(Local_SectorStart);
            if(Local_SectorState==0)
            {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
                BL_PrintMesssage("Successful Erase to Sectors From %d to %d\r\n",Copy_FirstSector,Copy_NumofSectors+Copy_FirstSector);
#endif
                Local_EraseState=true;
            }
            else
            {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
                BL_PrintMesssage("Failed to Erase Memory\r\n");
#endif
                Local_EraseState=false;
                break;
            }
        }
    }
    else
    {
        /*Do Nothing*/
    }
    return Local_EraseState;
}

/******************************************************************************
 * \Syntax          : uint8_t BL_RDPHelper(void)
 * \Description     : Read the Read Protection Level Helper Function
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : uint8_t
 *                    The Read Protection Level
 *******************************************************************************/
static uint8_t BL_RDPHelper(void)
{
    tFlashProtection Local_ProtectLevel;
    /*Get the Protection Level*/
    Local_ProtectLevel=FlashProtectGet(FLASH_BASE);
    return Local_ProtectLevel;
}

/************************************************************************************
 * \Syntax          : bool BL_ProgramRDPHelper(uint8_t Copy_PL)
 * \Description     : Helper function to set the protection Level of flash sector
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_PL:  Protection Level to be set
 * \Parameters (out): None
 * \Return value:   : bool
 *                    true - false
 ************************************************************************************/
static bool BL_ProgramRDPHelper(uint8_t Copy_PL)
{
    bool Local_ReadStatus=false;
    int32_t Local_Status;
    /*Set the flash protect*/
    Local_Status=FlashProtectSet(FLASH_BASE,(tFlashProtection) Copy_PL);
    /*Make sure the protect level changed successfully*/
    if(Local_Status==0)
    {
        Local_ReadStatus=true;
    }
    else
    {
        /*Do Nothing*/
    }
    return Local_ReadStatus;
}

/**************************************************************************************************************************
 * \Syntax          : bool BL_PerformFlashWrite(uint8_t* Copy_HostPayload,uint32_t Copy_StartAddress,uint8_t Copy_DataLen)
 * \Description     : Perform the write operation on flash
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_Address Address to be verified
 * \Parameters (out): None
 * \Return value:   : bool
 *                    true - false
 **************************************************************************************************************************/
static bool BL_PerformFlashWrite(uint8_t* Copy_HostPayload,uint32_t Copy_StartAddress,uint8_t Copy_DataLen)
{
    bool Local_WriteState=false;
    int32_t Local_Status=1;
    /*Make a pointer points to every 4 bits*/
    uint32_t *Local_Dataptr=((uint32_t*)Copy_HostPayload);
    /*Make Data multiple of four*/
    uint16_t Local_Number=Copy_DataLen%4;
    Local_Number+=Copy_DataLen;
    /*Make sure that flash writing done successfully*/
    Local_Status=FlashProgram(Local_Dataptr,Copy_StartAddress,(uint32_t)Local_Number);
    if(Local_Status==0)
    {
        Local_WriteState=true;
    }
    return Local_WriteState;
}

/******************************************************************************
 * \Syntax          : uint32_t Calculate_CRC32(const uint8_t *Buffer, uint8_t Buffer_Length)
 * \Description     : Calculate the CRC for Data Sent From Host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Buffer:        Data to Calculate the CRC for
 *                    Buffer_Length: Data Length
 * \Parameters (out): None
 * \Return value:   : uint32_t
 *                    The Calculated CRC
 *******************************************************************************/
static uint32_t Calculate_CRC32(const uint8_t *Buffer, uint8_t Buffer_Length)
{
    uint32_t CRC_Value = 0xFFFFFFFF;
    uint8_t i=0;
    uint8_t j=0;
    /*Calculate CRC with our Generator*/
    for (i = 0; i < Buffer_Length; ++i)
    {
        CRC_Value = CRC_Value ^ Buffer[i];
        for (j = 0; j < 32; ++j)
        {
            if (CRC_Value & 0x80000000)
            {
                CRC_Value = (CRC_Value << 1) ^ 0x04C11DB7;
            }
            else
            {
                CRC_Value = (CRC_Value << 1);
            }
        }
    }
    return CRC_Value;
}

/******************************************************************************
 * \Syntax          : bool BL_CRCCheck(void)
 * \Description     : Check for CRC Sent by the Host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_Address Address to be verified
 * \Parameters (out): None
 * \Return value:   : bool
 *                    true - false
 *******************************************************************************/
static bool BL_CRCCheck(void)
{
    bool Local_CRCState=false;
    uint8_t Local_DataLen=BL_HostBuffer[0]+1U;
    /*Get the host CRC from the sent Packet*/
    uint32_t Local_HostCRC=*((uint32_t*)((BL_HostBuffer+Local_DataLen)-BL_CRC_LEN));
    uint32_t Local_CalculatedCRC=0;
    /*CRC Verification*/
    Local_CalculatedCRC=Calculate_CRC32(BL_HostBuffer, (Local_DataLen-BL_CRC_LEN));
    if(Local_CalculatedCRC == Local_HostCRC)
    {
        Local_CRCState=true;
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("CRC Verification Passed\r\n");
#endif
    }
    else
    {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("CRC Verification Failed");
#endif
    }
    return Local_CRCState;
}

/*************************************************************************************************
 * \Syntax          : void BL_SendACK(uint8_t Copy_ReplyLen)
 * \Description     : Send ACK + Length to Follow to host in case of CRC verification Passed
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : Copy_ReplyLen : The Reply length to be followed by ACK
 * \Parameters (out): None
 * \Return value:   : void
 *************************************************************************************************/
static void BL_SendACK(uint8_t Copy_ReplyLen)
{
    uint8_t Local_AckVal[2]={0};
    Local_AckVal[0]=BL_ACK;
    Local_AckVal[1]=Copy_ReplyLen;
    /*Send ACK + Length to Flow to Host*/
    BL_SendDataToHost(Local_AckVal, 2U);
}

/******************************************************************************
 * \Syntax          : void BL_SendNACK(void)
 * \Description     : Send Not ACK to host in case of CRC verification failed
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_SendNACK(void)
{
    uint8_t Local_NACKVal=BL_NACK;
    /*Send Not ACK*/
    BL_SendDataToHost(&Local_NACKVal, 1U);
}

/******************************************************************************
 * \Syntax          : bool BL_AddressVerification(uint32_t Copy_Address)
 * \Description     : Verify the Address sent by host is in range of memory
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Reentrant
 * \Parameters (in) : Copy_Address Address to be verified
 * \Parameters (out): None
 * \Return value:   : bool
 *                    true - false
 *******************************************************************************/
static bool BL_AddressVerification(uint32_t Copy_Address)
{
    bool Local_AddState=false;
    /*Check if Address is inside the flash range or not*/
    if(((int32_t)Copy_Address>=FLASH_START_ADDRESS) && (Copy_Address<=FLASH_END_ADDRESS))
    {
        Local_AddState=true;
    }
    /*Check if Address is inside the SRAM range or not*/
    else if((Copy_Address>=SRAM_START_ADDRESS) && (Copy_Address<=SRAM_END_ADDRESS))
    {
        Local_AddState=true;
    }
    else
    {
        /* Do Nothing */
    }
    return Local_AddState;
}

/******************************************************************************
 * \Syntax          : void BL_JumpToUserAPP(void)
 * \Description     : Send the Bootloader version to host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_JumpToUserAPP(void)
{
    uint32_t Local_ResetHandlerAddress=*((volatile uint32_t*)(BL_AppAddress+4U));
    /*Move the Vector table to the start of application*/
    VTABLE_REG=BL_AppAddress;
    /*Pass the Reset Handler Address to pointer to function*/
    APP_ResetHandler=(void*)Local_ResetHandlerAddress;
    /*Initialize Stack Pointer*/
    __asm volatile("        MOV      R12,#0x0200");
    __asm volatile("        MOVT     R12,#0x2000");
    __asm volatile("        MSR      MSP,R12");
    /*Jump to Reset handler Address*/
    APP_ResetHandler();
}

/******************************************************************************
 * \Syntax          : void BL_GetVersion(void)
 * \Description     : Send the Bootloader version to host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_GetVersion(void)
{
    uint8_t Local_BLVersion[]={BL_VENDOR_ID,BL_SW_MAJOR_VERSION,BL_SW_MANOR_VERSION,BL_SW_PATCH_VERSION};
    if(BL_CRCCheck()==true)
    {
        /*Send ACK with 4 bytes as remaining length*/
        BL_SendACK(4U);
        /*Send the Version Info to Host*/
        BL_SendDataToHost(Local_BLVersion,4U);
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Read the Bootloader version %d. %d. %d\r\n",Local_BLVersion[1],Local_BLVersion[2],Local_BLVersion[3]);
#endif
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_GetHelp(void)
 * \Description     : Send the Commands codes to host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_GetHelp(void)
{
    uint8_t Local_BLCMD[]={BL_GET_VER,BL_GET_HELP,BL_GET_CID,BL_GET_RDP_LEVEL,BL_GO_TO_ADDR,BL_ERASE_FLASH,BL_WRITE_MEM,BL_ENABLE_DISABLE_WRP,BL_READ_MEM
                           ,BL_GET_WRP_STATUS,BL_READ_OTP,BL_SET_RDP_LEVEL,BL_JUMP_TO_USER_APP};
    if(BL_CRCCheck()==true)
    {
        /*Send ACK with 12 Bytes as Length to Follow*/
        BL_SendACK(13U);
        /*Send the Supported Commands*/
        BL_SendDataToHost(Local_BLCMD,13U);
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Read The commands supported by bootloader\r\n");
#endif
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_GetChipID(void)
 * \Description     : Get the chip Identification Number
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_GetChipID(void)
{
    /*Get Chip*/
    uint16_t Local_ChipID=(uint16_t)DID_REG;
    uint8_t Local_ChipIDBytes[2]={0};
    Local_ChipIDBytes[0]=(uint8_t)Local_ChipID;
    Local_ChipIDBytes[1]=Local_ChipID>>8;
    if(BL_CRCCheck()==true)
    {
        BL_SendACK(2U);
        BL_SendDataToHost(Local_ChipIDBytes,2U);
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Get the Chip Identification %d\r\n");
#endif
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_GoToAdd(void)
 * \Description     : Go to specific Memory Address
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_GoToAdd(void)
{
    bool Local_AddState=false;
    /*Get the Jump Address from Host Packet*/
    uint32_t Local_HostJumpAddress=*((uint32_t*)(BL_HostBuffer+2));
    void (*Local_JumpAddptr)(void)=NULL;
    if(BL_CRCCheck()==true)
    {
        BL_SendACK(1U);
        /*Verify the Address*/
        Local_AddState=BL_AddressVerification(Local_HostJumpAddress);
        /*Send the Verification Result to Host*/
        BL_SendDataToHost((uint8_t*)&Local_AddState, 1U);
        if(Local_AddState==true)
        {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
            BL_PrintMesssage("The address is valid. Jumping to %x\r\n",Local_HostJumpAddress);
#endif
            Local_JumpAddptr=(void*)(Local_HostJumpAddress);
            /*Jump to Address that the Host want*/
            Local_JumpAddptr();
        }
        else
        {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
            BL_PrintMesssage("The address isn't Valid\r\n");
#endif
        }
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_EraseFlash(void)
 * \Description     : Erase Number of blocks from flash
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_EraseFlash(void)
{
    bool Local_FlashEraseState=false;
    if(BL_CRCCheck()==true)
    {
        /*Set the Address Flag low to take the next Application base address when programming*/
        BL_AddFlag=0U;
        BL_SendACK(1U);
        /*Erase the number of sectors that host needs*/
        Local_FlashEraseState=BL_PerformFlashErase(BL_HostBuffer[2], BL_HostBuffer[3]);
        BL_SendDataToHost((uint8_t*)&Local_FlashEraseState, 1U);
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_WriteMem(void)
 * \Description     : Write Data to specific Memory Address
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_WriteMem(void)
{
    bool Local_AddressState=false;
    uint32_t Local_StartAddress=*((uint32_t*)(BL_HostBuffer+2));
    uint32_t Local_PayLoadLen= BL_HostBuffer[6];
    if(BL_CRCCheck()==true)
    {
        /*Get the First Address of the Application*/
        if(BL_AddFlag==0U)
        {
            /*Set the Flag to 1 to Make sure that we take the application base Address*/
            BL_AddFlag=1U;
            BL_AppAddress=Local_StartAddress;
        }
        BL_SendACK(1U);
        Local_AddressState=BL_AddressVerification(Local_StartAddress);
        if(Local_AddressState==true)
        {
            Local_AddressState=BL_PerformFlashWrite(&BL_HostBuffer[7],Local_StartAddress,Local_PayLoadLen);
        }
        else
        {

        }
        BL_SendDataToHost((uint8_t*)&Local_AddressState, 1U);
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_ReadProtectLevel(void)
 * \Description     : Get the Read Protect Level
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
static void BL_ReadProtectLevel(void)
{
    uint8_t Local_PRLevel=0U;
    if(BL_CRCCheck()==true)
    {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Read Protection Level\r\n");
#endif
        BL_SendACK(1U);
        /*Read the protect Level*/
        Local_PRLevel=BL_RDPHelper();
        BL_SendDataToHost(&Local_PRLevel, 1U);
    }
    else
    {
        BL_SendNACK();
    }
}

/******************************************************************************
 * \Syntax          : void BL_SetProtectLevel(void)
 * \Description     : Set the Read Protect Level
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
void BL_SetProtectLevel(void)
{
    bool Local_PRState=false;
    if(BL_CRCCheck()==true)
    {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Read Protection Level\r\n");
#endif
        BL_SendACK(1U);
        /*Set the Protect Level*/
        Local_PRState=BL_ProgramRDPHelper(BL_HostBuffer[2]);
        BL_SendDataToHost((uint8_t*)&Local_PRState, 1);
    }
    else
    {
        BL_SendNACK();
    }
}

static void BL_JUmpToUserAppCmd(void)
{
    bool Local_State=false;
    if(BL_CRCCheck()==true)
    {
#if BL_DUBUG_STATUS==BL_DUBUG_ON
        BL_PrintMesssage("Jumping to User Application\r\n");
#endif
        BL_SendACK(1U);
        /*Make sure that there is Application on the sector*/
        if(BL_AppAddress!=0U)
        {
            Local_State=true;
            /*Send the Jumping State*/
            BL_SendDataToHost((uint8_t*)&Local_State, 1);
            /*Jump to user Application*/
            BL_JumpToUserAPP();
        }
        else
        {
            BL_SendDataToHost((uint8_t*)&Local_State, 1);
        }
    }
    else
    {
        BL_SendNACK();
    }
}

/*TODO: : Future Work Implement this Function here and in host script*/
void BL_SetWriteProtoect(void)
{

}

/*TODO: : Future Work Implement this Function here and in host script*/
void BL_GetWriteProtoectState(void)
{

}
/*TODO: : Future Work Implement this Function here and in host script*/
void BL_ReadOTP(void)
{

}

/*TODO: : Future Work Implement this Function here and in host script*/
static void BL_ReadMem(void)
{

}

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/******************************************************************************
 * \Syntax          : void BL_FetchHostCommand(void)
 * \Description     : Fetch the command come from Host
 *
 * \Sync\Async      : Synchronous
 * \Reentrancy      : Non Reentrant
 * \Parameters (in) : None
 * \Parameters (out): None
 * \Return value:   : void
 *******************************************************************************/
void BL_FetchHostCommand(void)
{
    /*Receive the Packet Length from Host*/
    BL_ReceiveDataFromHost(&BL_HostBuffer[0], 1);
#if BL_DUBUG_STATUS==BL_DUBUG_ON
    BL_PrintMesssage("Bootloader Starts\r\n");
#endif
    /*Receive the Reset of packet from Host*/
    BL_ReceiveDataFromHost(&BL_HostBuffer[1], BL_HostBuffer[0]);
    BL_Command=BL_HostBuffer[1]-BL_GET_VER;
    if(BL_HostBuffer[1]<=BL_JUMP_TO_USER_APP)
    {
        /*Call the appropriate Function to Fetch the Command*/
        BL_FuncPtrArr[BL_Command]();
    }
    else
    {
        BL_PrintMesssage("Wrong Message ID From Host!!");
    }
}
/**********************************************************************************************************************
 *  END OF FILE: Bootloader.c
 *********************************************************************************************************************/
