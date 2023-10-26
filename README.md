# Bootloader Project README

This README provides an overview of the Bootloader Project designed for use with TIVA C microcontrollers. The Bootloader Project enables the loading of code onto the TIVA C microcontroller and offers 13 services that can be accessed from a Python host. The communication between the Python host and the microcontroller is established using UART, while the microcontroller communicates with the target device using CAN.

## Overview

The Bootloader Project allows for the updating of firmware on the TIVA C microcontroller by providing a dedicated bootloader program. The bootloader program runs on the microcontroller and facilitates the communication between a Python host and the target device. The Python host sends commands to the microcontroller via UART, and the microcontroller uses CAN to communicate with the target device.

## Services

The Bootloader Project includes the following 13 services, each associated with a specific command:

1. **BL_GET_VER_CMD**: Retrieves the bootloader version information.
2. **BL_GET_HELP_CMD**: Provides help information and lists the available commands.
3. **BL_GET_CID_CMD**: Retrieves the chip identifier information.
4. **BL_GET_RDP_STATUS_CMD**: Retrieves the Read Protection (RDP) status.
5. **BL_GO_TO_ADDR_CMD**: Directs the microcontroller to jump to a specific address.
6. **BL_FLASH_ERASE_CMD**: Erases a specific sector of the microcontroller's flash memory.
7. **BL_MEM_WRITE_CMD**: Writes data to a specific address in the microcontroller's memory.
8. **BL_ED_W_PROTECT_CMD**: Enables or disables write protection for specific memory sectors.
9. **BL_MEM_READ_CMD**: Reads data from a specific address in the microcontroller's memory.
10. **BL_READ_SECTOR_STATUS_CMD**: Retrieves the status of a specific sector in the microcontroller's memory.
11. **BL_OTP_READ_CMD**: Reads data from the microcontroller's One-Time Programmable (OTP) memory.
12. **BL_CHANGE_ROP_Level_CMD**: Changes the Read-Out Protection (ROP) level of the microcontroller.
13. **BL_JUMP_TO_USER_APP**: Directs the microcontroller to jump to the user application.

## Usage

To use the Bootloader Project, follow these steps:

1. Connect the TIVA C microcontroller to the target device using CAN.
2. Connect the TIVA C microcontroller to the Python host via UART.
3. Ensure that the Python host is set up with the necessary communication libraries for UART communication.
4. Build and flash the Bootloader Project onto the TIVA C microcontroller using the appropriate development environment.
5. Run the Python host script that communicates with the microcontroller via UART and sends commands to perform the desired operations.
6. The microcontroller will receive the commands from the Python host and execute the corresponding operations on the target device using CAN.
7. Monitor the progress and status of the operations through the Python host script.

## Compatibility

The Bootloader Project is specifically designed for use with TIVA C microcontrollers and supports communication with the target device using CAN. The Python host script communicates with the microcontroller through UART. Ensure that the TIVA C microcontroller, target device, and Python host are all properly configured and compatible for seamless operation.

## Documentation

For more detailed information on each service and its associated command, refer to the project documentation or source code comments. The documentation provides insights into the implementation details and usage guidelines for the Bootloader Project.

## Troubleshooting

If you encounter any issues while using the Bootloader Project, consider the following troubleshooting steps:

1. Verify the connections between the TIVA C microcontroller, target device, and Python host.
2. Ensure that the UART communication between the Python host and the microcontroller is functioning correctly.
3. Check the CAN communication between the microcontroller and the target device for proper configuration and connectivity.
4. Double-check the command syntax and ensure that the Python host is sending the correct commands to the microcontroller.
5. Verify the power supply to all components and ensure that they are properly powered on.
6. Debug the microcontroller code for any potential issues or errors in the implementation.
7. Consult the project documentation or seek assistance from the community or project maintainers.

## Conclusion

The Bootloader Project for TIVA C microcontrollers provides a convenient way to load code onto the microcontroller and offers a set of 13 services that can be accessed from a Python host. By utilizing UART for communication between the Python host and the microcontroller, and CAN for communication with the target device, the Bootloader Project simplifies the firmware update process. Follow the usage instructions and consult the documentation for further guidance on utilizing the Bootloader Project for your specific needs.
