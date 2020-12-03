#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static uint8_t myData[10240] = {};
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, myCalculatedCRC;

//![Simple CRC32 Example] 
int main(void)
{
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    uint32_t ii;

    for(ii = 0; ii < 10240; ii++){
        uint8_t random = rand() % 256;
        //if ii is even but random is not even or if ii is odd but random is not odd, subtract 1 to make it even
        if((ii % 2 == 0 && random % 2 != 0 || (ii % 2 == 1 && random % 2 == 0))){
            //mod 255 in case random is already 0 so you don't add -1 to array
            random = (random - 1) % 255;
        }
        myData[ii] = random;
    }

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);

    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t time_hw = (t0_hw-t1_hw) * 1000 / 3000000;

    /* Calculating the CRC32 checksum through software */
    uint32_t t0_sw = MAP_Timer32_getValue(TIMER32_0_BASE);
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 10240);
    uint32_t t1_sw = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t time_sw = (t0_sw-t1_sw) * 1000 / 3000000;

    uint32_t t0_myFunction = MAP_Timer32_getValue(TIMER32_0_BASE);
    myCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);
    uint32_t t1_myFunction = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t time_myFunction = (t0_myFunction-t1_myFunction) * 1000 / 3000000;

    printf("hw: %u\n", hwCalculatedCRC);
    printf("hw time: %ums\n", time_hw);
    printf("sw: %u\n", swCalculatedCRC);
    printf("sw time: %ums\n", time_sw);
    printf("my function: %u\n", myCalculatedCRC);
    printf("my function time: %ums\n", time_myFunction);

    /* Pause for the debugger */
    __no_operation();
}
//![Simple CRC32 Example] 

uint32_t compute_simple_checksum(uint8_t* data, uint32_t length){
    uint32_t ii, crc;
    crc = 0x0;
    for(ii = 0; ii < length; ii++){
        crc = crc + data[ii];
    }
    return ~crc;
}

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;;

    crc = 0xFFFFFFFF;

    for(ii=0;ii<length;ii++)
    {
        byte = data[ii];
        crc = crc ^ byte;

        for (jj = 0; jj < 8; jj++)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (CRC32_POLY & mask);
        }

    }

    return ~crc;
}
