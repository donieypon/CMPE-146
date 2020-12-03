/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "12345";
char string2[] = "12345";
const char string3[1024 * 150] = {"12345"};

// functions
uint32_t getBank(uint32_t addr);
uint32_t getSectorMask(uint32_t addr);

int main(void)
{

    // print all initial values
    printf("\nBefore modification: \n");
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string3: %s\n", string3);

    // modifying string2
    string2[0] = '0';
    string2[1] = '4';

    // convert memory location to bank number and sector mask
    // unprotect sectors
    FlashCtl_unprotectSector(getBank(string1), getSectorMask(string1));
    FlashCtl_unprotectSector(getBank(string3), getSectorMask(string3));

    // modifying string1 and string3
    ROM_FlashCtl_programMemory(string2, string1, 2);
    ROM_FlashCtl_programMemory(string2, string3, 2);

    printf("After modification: \n");
    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    printf("string3: %s\n", string3);

}

// Write a small function to convert a memory location to bank number and sector mask
/*
 * MSP432P401x:
 *  - Flash Main Memory: 64 sectors of 4KB
 *  - Flash INFO Memory: 4 sectors of 4KB
 */

uint32_t getBank(uint32_t addr) {

    // banks from datasheet
    if (addr >= 0x0001ffff && addr < 0x0003ffff) {
        return FLASH_MAIN_MEMORY_SPACE_BANK0;
    } else if (addr < 0x0001ffff) {
        return FLASH_MAIN_MEMORY_SPACE_BANK1;
    } else {
        return NULL;
    }
}

uint32_t getSectorMask(uint32_t addr) {

    // sectors from datasheet
    if (addr <= 0x0001ffff) {
        return ((addr - 0x00020000) / 4096);
    } else if (addr > 0x0001ffff && addr <= 0x0003ffff) {
        return ((addr - 0x00000000) / 4096);
    } else {
        return NULL;
    }
}
