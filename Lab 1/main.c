
/* Exercise 4
 *
 * Task: Read from the Device Descriptor Table (top 3 blocks, up to Die Record block)
 * - MCU datasheet address range: 00201000h-00201040h
 * - read and print the:
 *      - description
 *      - address
 *      - value in 8-digit hexadecimal
 *
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

int main(void)
{
    // counter for for loop
    volatile uint32_t i;

    // starting address of table
    uint32_t address = 0x00201000;

    const char* description_arr[17] = { "TLV checksum",
                                 "Device Info Tag", "Device Info Length", "Device ID", "Hardware Revision",
                                 "Boot-Code Revision", "ROM Driver Library Revision",
                                 "Die Record Tag", "Die Record Length", "Die X Position", "Die Y Position",
                                 "Wafer ID", "Lot ID", "Reserved", "Reserved", "Reserved", "Test Results"
                                };

    for(i=0; i<17; i++) {
        // pointer to address
        uint32_t* address_ptr = (uint32_t*)address;
        uint32_t data = *address_ptr;

        // print description, address, data
        printf("description: %s | address: %08xh | data: %08xh \n", description_arr[i], address_ptr, data);

        // increment address to next location
        address = address + 4;
    }

}
