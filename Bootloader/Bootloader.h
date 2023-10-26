/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *       Author:  Mahmoud Badr
 *         File:  Bootloader.h
 *        Layer:  App
 *       Module:  Bootloader
 *      Version:  1.00
 *
 *  Description:  -
 *
 *********************************************************************************************************************/

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/
#define FLASH_START_ADDRESS     FLASH_BASE
#define FLASH_END_ADDRESS       (FLASH_BASE+256UL*1024)
#define SRAM_START_ADDRESS      SRAM_BASE
#define SRAM_END_ADDRESS        (SRAM_BASE+32*1024UL)
#define FLASH_SECTOR_SIZE       1024UL
#define BL_HOST_BUFFER_SIZE     300U

/* Set the Debug State
 * BL_DEBUG_OFF
 * BL_DUBUG_ON */
#define BL_DUBUG_STATUS         BL_DUBUG_OFF

/* Set the Dubug Method
 * BL_UART_DEBUG
 * BL_CAN_DUBUG  */
#define BL_DEBUG_METHODE        BL_UART_DEBUG

/* Set the Debug UART Peripheral
 * UART_0
 * UART_1
 * UART_2
 * UART_3
 * UART_4
 * UART_5
 * UART_6
 * UART_7 */
#define BL_DEBUG_UART           UART_0

/* Set the Communication Protocol with Host
 * BL_UART_COMM
 * BL_CAN_COMM  */
#define BL_COMM_PROTOCOL        BL_UART_COMM

/* Set the Communication Method
 * UART_0:7
 * CAN0 : 1 */
#define BL_COMM_METHODE         UART_0

/**********************************************************************************************************************
 *  LOCAL MACROS CONSTANT\FUNCTION
 *********************************************************************************************************************/
#define BL_GET_VER              0x10
#define BL_GET_HELP             0x11
#define BL_GET_CID              0x12
#define BL_GET_RDP_LEVEL        0x13
#define BL_GO_TO_ADDR           0x14
#define BL_ERASE_FLASH          0x15
#define BL_WRITE_MEM            0x16
#define BL_ENABLE_DISABLE_WRP   0x17
#define BL_READ_MEM             0x18
#define BL_GET_WRP_STATUS       0x19
#define BL_READ_OTP             0x1A
#define BL_SET_RDP_LEVEL        0x1B
#define BL_JUMP_TO_USER_APP     0x1C

#define BL_MASS_ERASE           0xff

#define BL_VENDOR_ID            0x10
#define BL_SW_MAJOR_VERSION     1U
#define BL_SW_MANOR_VERSION     0U
#define BL_SW_PATCH_VERSION     0U

#define BL_ACK                  0xCD
#define BL_NACK                 0xAB

#define BL_UART_DEBUG           0x01
#define BL_CAN_DUBUG            0x02
#define BL_DEBUG_OFF            0x01
#define BL_DUBUG_ON             0x02
#define BL_UART_COMM            0x01
#define BL_CAN_COMM             0x02
#define BL_CRC_LEN              4U
#define BL_FLASH_SECTORS_NUM    256U

#define DID_REG                 ((*((uint32_t*)(SYSCTL_BASE+4U)))>>16U)
#define VTABLE_REG              (*((volatile unsigned int*)0xE000ED08))

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
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
static void BL_PrintMesssage(char *Copy_Format,...);

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
static void BL_SendDataToHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen);

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
static void BL_ReceiveDataFromHost(uint8_t* Copy_HostBuffer,uint8_t Copy_DataLen);

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
static bool BL_PerformFlashErase(uint8_t Copy_FirstSector,uint8_t Copy_NumofSectors);

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
static uint8_t BL_RDPHelper(void);

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
static bool BL_ProgramRDPHelper(uint8_t Copy_PL);

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
static bool BL_PerformFlashWrite(uint8_t* Copy_HostPayload,uint32_t Copy_StartAddress,uint8_t Copy_DataLen);

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
static uint32_t Calculate_CRC32(const uint8_t *Buffer, uint8_t Buffer_Length);

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
static bool BL_CRCCheck(void);

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
static void BL_SendACK(uint8_t Copy_ReplyLen);

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
static void BL_SendNACK(void);

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
static bool BL_AddressVerification(uint32_t Copy_Address);

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
static void BL_JumpToUserAPP(void);

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
static void BL_GetVersion(void);

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
static void BL_GetHelp(void);

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
static void BL_GetChipID(void);

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
static void BL_GoToAdd(void);

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
static void BL_EraseFlash(void);

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
static void BL_WriteMem(void);

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
static void BL_ReadProtectLevel(void);

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
void BL_SetProtectLevel(void);

static void BL_JUmpToUserAppCmd(void);

/*TODO: : Future Work Implement this Function here and in host script*/
void BL_SetWriteProtoect(void);

/*TODO: : Future Work Implement this Function here and in host script*/
void BL_GetWriteProtoectState(void);

/*TODO: : Future Work Implement this Function here and in host script*/
void BL_ReadOTP(void);

/*TODO: : Future Work Implement this Function here and in host script*/
static void BL_ReadMem(void);

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
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
void BL_FetchHostCommand(void);

#endif
