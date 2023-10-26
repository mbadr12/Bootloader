import serial
import struct
import os
import sys
import glob
from time import sleep

''' Bootloader Commands '''
BL_GET_VER_CMD              = 0x10
BL_GET_HELP_CMD             = 0x11
BL_GET_CID_CMD              = 0x12
BL_GET_RDP_STATUS_CMD       = 0x13
BL_GO_TO_ADDR_CMD           = 0x14
BL_FLASH_ERASE_CMD          = 0x15
BL_MEM_WRITE_CMD            = 0x16
BL_ED_W_PROTECT_CMD         = 0x17
BL_MEM_READ_CMD             = 0x18
BL_READ_SECTOR_STATUS_CMD   = 0x19
BL_OTP_READ_CMD             = 0x1A
BL_CHANGE_ROP_Level_CMD     = 0x1B
BL_JUMP_TO_USER_APP         = 0x1C

INVALID_SECTOR_NUMBER        = 0x00
VALID_SECTOR_NUMBER          = 0x01
UNSUCCESSFUL_ERASE           = 0x00
SUCCESSFUL_ERASE             = 0x01

FLASH_PAYLOAD_WRITE_FAILED   = 0x00
FLASH_PAYLOAD_WRITE_PASSED   = 0x01

verbose_mode = 1
Memory_Write_Active = 0

def Check_Serial_Ports():
    Serial_Ports = []
    if sys.platform.startswith('win'):
        Ports = ['COM%s' % (i + 1) for i in range(256)]
    else:
        raise EnvironmentError("Error !! Unsupported Platform \n")
    
    for Serial_Port in Ports:
        try:
            test = serial.Serial(Serial_Port)
            test.close()
            Serial_Ports.append(Serial_Port)
        except (OSError, serial.SerialException):
            pass
    
    return Serial_Ports

def Serial_Port_Configuration(Port_Number):
    global Serial_Port_Obj
    try:
        Serial_Port_Obj = serial.Serial(Port_Number, 115200, timeout = 2)
    except:
        print("\nError !! That was not a valid port")
    
        Port_Number = Check_Serial_Ports()
        if(not Port_Number):
            print("\nError !! No ports Detected")
        else:
            print("\nHere are some available ports on your PC. Try Again !!")
            print("\n   ", Port_Number)
        return -1
    
    if Serial_Port_Obj.is_open:
        print("Port Open Success \n")
    else:
        print("Port Open Failed \n")
        return -1

def Write_Data_To_Serial_Port(Value, Length):
    _data = struct.pack('>B', Value)
    if(verbose_mode):
        Value = bytearray(_data)
        print("   "+"0x{:02x}".format(Value[0]), end = ' ')
        if(Memory_Write_Active and (not verbose_mode)):
            print("#", end = ' ')
        Serial_Port_Obj.write(_data)

def Read_Serial_Port(Data_Len):
    
    Serial_Value = Serial_Port_Obj.read(Data_Len)
    Serial_Value_len = len(Serial_Value)
    while Serial_Value_len <= 0:
        Serial_Value = Serial_Port_Obj.read(Data_Len)
        Serial_Value_len = len(Serial_Value)
        print("Waiting Replay from the Bootloader")
    return Serial_Value
    
    '''
    Serial_Value = Serial_Port_Obj.read(Data_Len)
    return Serial_Value
    '''

def Read_Data_From_Serial_Port(Command_Code):
    Length_To_Follow = 0
    
    BL_ACK = Read_Serial_Port(2)
    if(len(BL_ACK)):
        BL_ACK_Array = bytearray(BL_ACK)
        if(BL_ACK_Array[0] == 0xCD):
            print ("\n   Received Acknowledgement from Bootloader")
            Length_To_Follow = BL_ACK_Array[1]
            print("   Preparing to receive (", int(Length_To_Follow), ") bytes from the bootloader")
            if(Command_Code == BL_GET_VER_CMD):
                Process_BL_GET_VER_CMD(Length_To_Follow)
            elif (Command_Code == BL_GET_HELP_CMD):
                Process_BL_GET_HELP_CMD(Length_To_Follow)
            elif (Command_Code == BL_GET_CID_CMD):
                Process_BL_GET_CID_CMD(Length_To_Follow)
            elif (Command_Code == BL_GET_RDP_STATUS_CMD):
                Process_BL_GET_RDP_STATUS_CMD(Length_To_Follow)
            elif (Command_Code == BL_GO_TO_ADDR_CMD):
                Process_BL_GO_TO_ADDR_CMD(Length_To_Follow)
            elif (Command_Code == BL_FLASH_ERASE_CMD):
                Process_BL_FLASH_ERASE_CMD(Length_To_Follow)
            elif (Command_Code == BL_MEM_WRITE_CMD):
                Process_BL_MEM_WRITE_CMD(Length_To_Follow)
            elif (Command_Code == BL_CHANGE_ROP_Level_CMD):
                Process_BL_CHANGE_ROP_Level_CMD(Length_To_Follow)
            elif (Command_Code==BL_JUMP_TO_USER_APP):
                Process_BL_Jump_To_User_App(Length_To_Follow)
        else:
            print ("\n   Received Not-Acknowledgement from Bootloader")
            sys.exit()
        
def Process_BL_GET_VER_CMD(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    _value_ = bytearray(Serial_Data)
    print("\n   Bootloader Vendor ID : ", _value_[0])
    print("   Bootloader Version   : ", _value_[1], ".", _value_[2], ".", _value_[3])

def Process_BL_GET_HELP_CMD(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    _value_ = bytearray(Serial_Data)
    print("\n   Supported Commands : ")
    print("   /*****************************/")
    for command in _value_:
        if command==BL_GET_VER_CMD:
            print("   BL_GET_VER_CMD              -->",end = ' ')
        elif command==BL_GET_HELP_CMD:
            print("   BL_GET_HELP_CMD             -->", end = ' ')
        elif command==BL_GET_CID_CMD:    
            print("   BL_GET_CID_CMD              -->", end = ' ')
        elif command==BL_GET_RDP_STATUS_CMD:
            print("   BL_GET_RDP_STATUS_CMD       -->", end = ' ')
        elif command==BL_GO_TO_ADDR_CMD:
            print("   BL_GO_TO_ADDR_CMD           -->", end = ' ')
        elif command==BL_FLASH_ERASE_CMD:
            print("   BL_FLASH_ERASE_CMD          -->", end = ' ')
        elif command==BL_MEM_WRITE_CMD:
            print("   BL_MEM_WRITE_CMD            -->", end = ' ')
        elif command==BL_ED_W_PROTECT_CMD:
            print("   BL_ED_W_PROTECT_CMD         -->", end = ' ')
        elif command==BL_MEM_READ_CMD:
            print("   BL_MEM_READ_CMD             -->", end = ' ')
        elif command==BL_READ_SECTOR_STATUS_CMD:
            print("   BL_READ_SECTOR_STATUS_CMD   -->", end = ' ')
        elif command==BL_OTP_READ_CMD:
            print("   BL_OTP_READ_CMD             -->", end = ' ')
        elif command==BL_CHANGE_ROP_Level_CMD:
            print("   BL_CHANGE_ROP_Level_CMD     -->", end = ' ')
        elif command==BL_JUMP_TO_USER_APP:
            print("   BL_JUMP_TO_USER_APP         -->", end = ' ')
        print(hex(command))

def Process_BL_GET_CID_CMD(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    CID = (Serial_Data[1] << 8) | Serial_Data[0]
    print("\n   Chip Identification Number : ", hex(CID))

def Process_BL_GET_RDP_STATUS_CMD(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    _value_ = bytearray(Serial_Data)
    if(_value_[0] == 0xEE):
        print("\n   Error While Reading FLASH Protection level !!")
    elif(_value_[0] == 0x00):
        print("\n   FLASH Protection : LEVEL 0")
    elif(_value_[0] == 0x01):
        print("\n   FLASH Protection : LEVEL 1")
    elif(_value_[0] == 0x02):
        print("\n   FLASH Protection : LEVEL 2")

def Process_BL_GO_TO_ADDR_CMD(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    _value_ = bytearray(Serial_Data)
    if(_value_[0] == 1):
        print("\n   Address Status is Valid")
    else:
        print("\n   Address Status is InValid")

def Process_BL_FLASH_ERASE_CMD(Data_Len):
    BL_Erase_Status = 0
    Serial_Data = Read_Serial_Port(Data_Len)
    if(len(Serial_Data)):
        BL_Erase_Status = bytearray(Serial_Data)
        if(BL_Erase_Status[0] == INVALID_SECTOR_NUMBER):
            print("\n   Erase Status -> Invalid Sector Number ")
        elif (BL_Erase_Status[0] == UNSUCCESSFUL_ERASE):
            print("\n   Erase Status -> Unsuccessful Erase ")
        elif (BL_Erase_Status[0] == SUCCESSFUL_ERASE):
            print("\n   Erase Status -> successful Erase ")
        else:
            print("\n   Erase Status -> Unknown Error")
    else:
        print("Timeout !!, Bootloader is not responding")

def Process_BL_MEM_WRITE_CMD(Data_Len):
    global Memory_Write_All
    BL_Write_Status = 0
    Serial_Data = Read_Serial_Port(Data_Len)
    BL_Write_Status = bytearray(Serial_Data)
    if(BL_Write_Status[0] == FLASH_PAYLOAD_WRITE_FAILED):
        print("\n   Write Status -> Write Failed or Invalid Address ")
    elif (BL_Write_Status[0] == FLASH_PAYLOAD_WRITE_PASSED):
        print("\n   Write Status -> Write successful ")
        Memory_Write_All = Memory_Write_All and FLASH_PAYLOAD_WRITE_PASSED
    else:
        print("Timeout !!, Bootloader is not responding")

def Process_BL_CHANGE_ROP_Level_CMD(Data_Len):
    BL_CHANGE_ROP_Level_Status = 0
    Serial_Data = Read_Serial_Port(Data_Len)
    if(len(Serial_Data)):
        BL_CHANGE_ROP_Level_Status = bytearray(Serial_Data)
        if(BL_CHANGE_ROP_Level_Status[0] == 0x01):
            print("\n   ROP Level Changed")
        elif (BL_CHANGE_ROP_Level_Status[0] == 0x00):
            print("\n   ROP Level Not Changed ")
        else:
            print("\n   ROP Level -> Unknown Error")

def Process_BL_Jump_To_User_App(Data_Len):
    Serial_Data = Read_Serial_Port(Data_Len)
    _value_ = bytearray(Serial_Data)
    if(_value_[0] == 0x01):
        print("\n   Jump to user Application Successfully !!")
    elif(_value_[0] == 0x00):
        print("\n   There is No Application Burned Yet !!")
        
def Calculate_CRC32(Buffer, Buffer_Length):
    CRC_Value = 0xFFFFFFFF
    for DataElem in Buffer[0:Buffer_Length]:
        CRC_Value = CRC_Value ^ DataElem
        for DataElemBitLen in range(32):
            if(CRC_Value & 0x80000000):
                CRC_Value = (CRC_Value << 1) ^ 0x04C11DB7
            else:
                CRC_Value = (CRC_Value << 1)
    return CRC_Value
    
def Word_Value_To_Byte_Value(Word_Value, Byte_Index, Byte_Lower_First):
    Byte_Value = (Word_Value >> (8 * (Byte_Index - 1)) & 0x000000FF)
    return Byte_Value

def CalulateBinFileLength():
    BinFileLength = os.path.getsize("Application.bin")
    return BinFileLength

def OpenBinFile():
    global BinFile
    BinFile = open('Application.bin', 'rb')

def Decode_BL_Command(Command):
    BL_Host_Buffer = []
    BL_Return_Value = 0
    
    ''' Clear the bootloader host buffer '''
    for counter in range(255):
        BL_Host_Buffer.append(0)
    
    if(Command == 1):
        print("Request the bootloader version")
        BL_GET_VER_CMD_Len = 6
        BL_Host_Buffer[0] = BL_GET_VER_CMD_Len - 1
        BL_Host_Buffer[1] = BL_GET_VER_CMD
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_GET_VER_CMD_Len - 4)
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        print("Host CRC = ", hex(CRC32_Value))
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_GET_VER_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_GET_VER_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_GET_VER_CMD)
    elif (Command == 2):
        print("Read the commands supported by the bootloader")
        BL_GET_HELP_CMD_Len = 6
        BL_Host_Buffer[0] = BL_GET_HELP_CMD_Len - 1
        BL_Host_Buffer[1] = BL_GET_HELP_CMD
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_GET_HELP_CMD_Len - 4)
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_GET_HELP_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_GET_HELP_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_GET_HELP_CMD)
    elif (Command == 3):
        print("Read the MCU chip identification number")
        BL_GET_CID_CMD_Len = 6
        BL_Host_Buffer[0] = BL_GET_CID_CMD_Len - 1
        BL_Host_Buffer[1] = BL_GET_CID_CMD
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_GET_CID_CMD_Len - 4)
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_GET_CID_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_GET_CID_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_GET_CID_CMD)
    elif (Command == 4):
        print("Read the FLASH Read Protection level")
        BL_GET_RDP_STATUS_CMD_Len = 6
        BL_Host_Buffer[0] = BL_GET_RDP_STATUS_CMD_Len - 1
        BL_Host_Buffer[1] = BL_GET_RDP_STATUS_CMD
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_GET_RDP_STATUS_CMD_Len - 4)
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_GET_RDP_STATUS_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_GET_RDP_STATUS_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_GET_RDP_STATUS_CMD)
    elif (Command == 5):
        print("Jump bootloader to specified address command")
        BL_GO_TO_ADDR_CMD_Len = 10
        BL_Jump_Address = input("\n   Please Enter the Address in Hex : ")
        BL_Jump_Address = int(BL_Jump_Address, 16)
        BL_Host_Buffer[0] = BL_GO_TO_ADDR_CMD_Len - 1
        BL_Host_Buffer[1] = BL_GO_TO_ADDR_CMD
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(BL_Jump_Address, 1, 1) 
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(BL_Jump_Address, 2, 1) 
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(BL_Jump_Address, 3, 1) 
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(BL_Jump_Address, 4, 1)
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_GO_TO_ADDR_CMD_Len - 4) 
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[6] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[7] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[8] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[9] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_GO_TO_ADDR_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_GO_TO_ADDR_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_GO_TO_ADDR_CMD)
    elif (Command == 6):
        print("Mass erase or sector erase of the user flash command")
        BL_FLASH_ERASE_CMD_Len = 8
        SectorNumber = 0
        NumberOfSectors = 0
        BL_Host_Buffer[0] = BL_FLASH_ERASE_CMD_Len - 1
        BL_Host_Buffer[1] = BL_FLASH_ERASE_CMD
        SectorNumber = input("\n   Please enter start sector number(0-255)          : ")
        SectorNumber = int(SectorNumber, 16)
        if(SectorNumber != 0xFF):
            NumberOfSectors = int(input("\n   Please enter number of sectors to erase (255 Max): "), 16)
        BL_Host_Buffer[2] = SectorNumber
        BL_Host_Buffer[3] = NumberOfSectors
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_FLASH_ERASE_CMD_Len - 4) 
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[6] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[7] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_FLASH_ERASE_CMD_Len]:
            Write_Data_To_Serial_Port(Data, BL_FLASH_ERASE_CMD_Len - 1)
        Read_Data_From_Serial_Port(BL_FLASH_ERASE_CMD)
    elif (Command == 7):
        print("Write data into different memories of the MCU command")
        global Memory_Write_Is_Active
        global Memory_Write_All
        File_Total_Len = 0
        BinFileRemainingBytes = 0
        BinFileSentBytes = 0
        BaseMemoryAddress = 0
        BinFileReadLength = 0
        Memory_Write_All = 1
        
        ''' Get the total length of the binary file '''
        File_Total_Len = CalulateBinFileLength()
        print("   Preparing writing a binary file with length (", File_Total_Len, ") Bytes")
        ''' Open the binary file '''
        OpenBinFile()
        ''' Calculate the remaining payload '''
        BinFileRemainingBytes = File_Total_Len - BinFileSentBytes
        ''' Get the start address to write the payload '''
        BaseMemoryAddress = input("\n   Enter the start address : ")
        BaseMemoryAddress = int(BaseMemoryAddress, 16)
        ''' Keep sending the write packet till the last payload byte '''
        while(BinFileRemainingBytes):
            ''' Memory write is active '''
            Memory_Write_Is_Active = 1
            
            ''' Read 128 bytes from the binary file each time '''
            if(BinFileRemainingBytes >= 128):
                BinFileReadLength = 128
            else:
                BinFileReadLength = BinFileRemainingBytes
            
            for BinFileByte in range(BinFileReadLength):
                BinFileByteValue = BinFile.read(1)
                BinFileByteValue = bytearray(BinFileByteValue)
                BL_Host_Buffer[7 + BinFileByte] = int(BinFileByteValue[0])
            
            ''' Update the Host packet with the command code ID '''
            BL_Host_Buffer[1] = BL_MEM_WRITE_CMD
        
            ''' Update the Host packet with the base address '''
            BL_Host_Buffer[2] = Word_Value_To_Byte_Value(BaseMemoryAddress, 1, 1)
            BL_Host_Buffer[3] = Word_Value_To_Byte_Value(BaseMemoryAddress, 2, 1)
            BL_Host_Buffer[4] = Word_Value_To_Byte_Value(BaseMemoryAddress, 3, 1)
            BL_Host_Buffer[5] = Word_Value_To_Byte_Value(BaseMemoryAddress, 4, 1)
            
            ''' Update the Host packet with the payload length '''
            BL_Host_Buffer[6] = BinFileReadLength
            
            ''' Update the Host packet with the packet length '''
            BL_MEM_WRITE_CMD_Len = (BinFileReadLength + 11)
            BL_Host_Buffer[0] = BL_MEM_WRITE_CMD_Len - 1
            
            ''' Update the Host packet with the calculated CRC32 '''
            CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_MEM_WRITE_CMD_Len - 4) 
            CRC32_Value = CRC32_Value & 0xFFFFFFFF
            BL_Host_Buffer[7 + BinFileReadLength] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
            BL_Host_Buffer[8 + BinFileReadLength] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
            BL_Host_Buffer[9 + BinFileReadLength] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
            BL_Host_Buffer[10+ BinFileReadLength] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
            
            ''' Calculate the next Base memory address '''
            BaseMemoryAddress = BaseMemoryAddress + BinFileReadLength
            
            ''' Send the packet length to the bootloader '''
            Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
            
            ''' Send the complete packet to the bootloader '''
            for Data in BL_Host_Buffer[1 : BL_MEM_WRITE_CMD_Len]:
                Write_Data_To_Serial_Port(Data, BL_MEM_WRITE_CMD_Len - 1)
            
            ''' Update the total number of bytes sent to the bootloader '''
            BinFileSentBytes = BinFileSentBytes + BinFileReadLength
            
            ''' Calculate the remaining payload '''
            BinFileRemainingBytes = File_Total_Len - BinFileSentBytes
            print("\n   Bytes sent to the bootloader :{0}".format(BinFileSentBytes))
            
            ''' Read the response from the bootloader '''
            BL_Return_Value = Read_Data_From_Serial_Port(BL_MEM_WRITE_CMD)
            sleep(0.1)
        ''' Memory write is inactive '''
        Memory_Write_Is_Active = 0
        if(Memory_Write_All == 1):
            print("\n\n Payload Written Successfully")
    elif (Command == 12):
        print("Change read protection level of the user flash command")
        Protection_level = input("\n   Please Enter one of these Protection levels : 0,1,2 : ")
        Protection_level = int(Protection_level, 8)
        if(Protection_level == 2):
            print("\n   Protection level (2) not supported !!")
        elif(Protection_level == 0 or Protection_level == 1):
            print("\n   Changing the protection level to be : ", Protection_level)
            BL_CHANGE_ROP_Level_CMD_Len = 7
            BL_Host_Buffer[0] = BL_CHANGE_ROP_Level_CMD_Len - 1
            BL_Host_Buffer[1] = BL_CHANGE_ROP_Level_CMD
            if Protection_level==0:
                BL_Host_Buffer[2] = 0x00;
            else:
                BL_Host_Buffer[2] = 0x01;
            CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_CHANGE_ROP_Level_CMD_Len - 4) 
            CRC32_Value = CRC32_Value & 0xFFFFFFFF
            BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
            BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
            BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
            BL_Host_Buffer[6] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
            Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
            for Data in BL_Host_Buffer[1 : BL_CHANGE_ROP_Level_CMD_Len]:
                Write_Data_To_Serial_Port(Data, BL_CHANGE_ROP_Level_CMD_Len - 1)
            Read_Data_From_Serial_Port(BL_CHANGE_ROP_Level_CMD)
        else:
            print("\n   Protection level (", Protection_level, ") not supported !!")
    elif Command==13:
        print("Jumping to User APP")
        BL_JUMP_TO_USER_APP_LEN=6
        BL_Host_Buffer[0] = BL_JUMP_TO_USER_APP_LEN - 1
        BL_Host_Buffer[1] = BL_JUMP_TO_USER_APP
        CRC32_Value = Calculate_CRC32(BL_Host_Buffer, BL_JUMP_TO_USER_APP_LEN - 4) 
        CRC32_Value = CRC32_Value & 0xFFFFFFFF
        BL_Host_Buffer[2] = Word_Value_To_Byte_Value(CRC32_Value, 1, 1)
        BL_Host_Buffer[3] = Word_Value_To_Byte_Value(CRC32_Value, 2, 1)
        BL_Host_Buffer[4] = Word_Value_To_Byte_Value(CRC32_Value, 3, 1)
        BL_Host_Buffer[5] = Word_Value_To_Byte_Value(CRC32_Value, 4, 1)
        Write_Data_To_Serial_Port(BL_Host_Buffer[0], 1)
        for Data in BL_Host_Buffer[1 : BL_JUMP_TO_USER_APP_LEN]:
            Write_Data_To_Serial_Port(Data, BL_JUMP_TO_USER_APP_LEN - 1)
        Read_Data_From_Serial_Port(BL_JUMP_TO_USER_APP)
SerialPortName = input("Enter the Port Name of your device(Ex: COM3):")
State=Serial_Port_Configuration(SerialPortName)
if State!=-1:  
    while True:
        print("\nTM4c123xx Custome BootLoader")
        print("==============================")
        print("Which command you need to send to the bootLoader :");
        print("   BL_GET_VER_CMD              --> 1")
        print("   BL_GET_HELP_CMD             --> 2")
        print("   BL_GET_CID_CMD              --> 3")
        print("   BL_GET_RDP_STATUS_CMD       --> 4")
        print("   BL_GO_TO_ADDR_CMD           --> 5")
        print("   BL_FLASH_ERASE_CMD          --> 6")
        print("   BL_MEM_WRITE_CMD            --> 7")
        print("   BL_ED_W_PROTECT_CMD         --> 8")
        print("   BL_MEM_READ_CMD             --> 9")
        print("   BL_READ_SECTOR_STATUS_CMD   --> 10")
        print("   BL_OTP_READ_CMD             --> 11")
        print("   BL_CHANGE_ROP_Level_CMD     --> 12")
        print("   BL_JUMP_TO_USER_APP         --> 13")
        
        BL_Command = input("\nEnter the command code : ")
        
        if(not BL_Command.isdigit()):
            print("   Error !!, Please enter a valid command !! \n")
        else:
            Decode_BL_Command(int(BL_Command))
        
        input("\nPlease press any key to continue ...")
        Serial_Port_Obj.reset_input_buffer()