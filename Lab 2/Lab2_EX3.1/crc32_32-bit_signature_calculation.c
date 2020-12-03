#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

static uint8_t myData[10240];

static uint32_t calculateCRC32(uint8_t* data, uint32_t length);
static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC;
volatile uint32_t swCalculatedCRC;
volatile uint32_t myCalculatedCRC;

//![Simple CRC32 Example] 
int main(void)
{
    // initialize timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    int j;

    // fill myData with random numbers
    for (j = 0; j < 10240; j++) {
        // numbers must be alternating odd/even
        if (j % 2 == 0) {
            myData[j] = (2*(rand()%127)+1); // random odd integer
        } else {
            myData[j] = (2*(rand()%127)); // random even integer
        }
    }

    uint32_t ii;

    /* Stop WDT */
    MAP_WDT_A_holdTimer();

    /* ------------------- hardware ----------------------- */

    uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE); // sets seed for generating checksum

    for (ii = 0; ii < 10240; ii++)
        MAP_CRC32_set8BitData(myData[ii], CRC32_MODE); // send data byte one by one

    /* Getting the result from the hardware module */
    hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF; // read checksum

    uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_hw = (t0_hw-t1_hw) * 1000 / 3000000 * 1000;

    /* ---------------------------------------------------- */

    /* ------------------- software ----------------------- */

    uint32_t t0_sw = MAP_Timer32_getValue(TIMER32_0_BASE);

    /* Calculating the CRC32 checksum through software */
    swCalculatedCRC = calculateCRC32((uint8_t*) myData, 10240);

    uint32_t t1_sw = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_sw = (t0_sw-t1_sw) * 1000 / 3000000 * 1000;

    /* ---------------------------------------------------- */

    /* ------------------- my function ----------------------- */

    // calculating the CRC32 checksum through written function
    uint32_t t0_myFunc = MAP_Timer32_getValue(TIMER32_0_BASE);

    myCalculatedCRC = compute_simple_checksum((uint8_t*) myData, 10240);

    uint32_t t1_myFunc = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_myFunc = (t0_myFunc-t1_myFunc) * 1000 / 3000000 * 1000;

    /* ---------------------------------------------------- */

    // calculating speedup
    // how much fast is hw wrt sw
    uint32_t speedup = duration_sw/duration_hw;

    /* ------------------------ Ex. 3.4 -------------------------- */

    // select a data point
    uint32_t dataPoint = myData[20];

    // reverse least significant bit
    myData[20] = dataPoint ^ 0x1;

    // compute checksums
    // hw
    uint32_t t0_hw_rev = MAP_Timer32_getValue(TIMER32_0_BASE);

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE); // sets seed for generating checksum
    for (ii = 0; ii < 10240; ii++)
           MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);
    uint32_t hw_rev_CalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    uint32_t t1_hw_rev = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_hw_rev = (t0_hw_rev-t1_hw_rev) * 1000 / 3000000 * 1000;

    // my function
    uint32_t t0_myFunc_rev = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t myCalculatedCRC_rev = compute_simple_checksum((uint8_t*) myData, 10240);
    uint32_t t1_myFunc_rev = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_myFunc_rev = (t0_myFunc_rev-t1_myFunc_rev) * 1000 / 3000000 * 1000;

    // reverse least significant bit of myData[21]
    uint32_t dataPoint2 = myData[21];
    myData[21] = dataPoint2 ^ 0x1;

    // compute checksums
    // hw
    uint32_t t0_hw_rev2 = MAP_Timer32_getValue(TIMER32_0_BASE);

    MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE); // sets seed for generating checksum
    for (ii = 0; ii < 10240; ii++)
           MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);
    uint32_t hw_rev_CalculatedCRC2 = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;

    uint32_t t1_hw_rev2 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_hw_rev2 = (t0_hw_rev2-t1_hw_rev2) * 1000 / 3000000 * 1000;

    // my function
    uint32_t t0_myFunc_rev2 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t myCalculatedCRC_rev2 = compute_simple_checksum((uint8_t*) myData, 10240);
    uint32_t t1_myFunc_rev2 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t duration_myFunc_rev2 = (t0_myFunc_rev2-t1_myFunc_rev2) * 1000 / 3000000 * 1000;

    /* ---------------------------------------------------- */

    printf("hw calculated CRC: %u\n", hwCalculatedCRC);
    printf("hw duration: %uus\n", duration_hw);

    printf("sw calculated CRC: %u\n", swCalculatedCRC);
    printf("sw duration: %uus\n", duration_sw);

    printf("my function's calculated CRC: %u\n", myCalculatedCRC);
    printf("my function's duration: %uus\n", duration_myFunc);

//    printf("Speedup of using accelerator over sw: %ux faster\n", speedup);

    printf("myData[20] hw checksum: %u\n", hw_rev_CalculatedCRC);
    printf("myData[20] hw duration: %uus\n", duration_hw_rev);

    printf("myData[20] myFunc checksum: %u\n", myCalculatedCRC_rev);
    printf("myData[20] myFunc duration: %uus\n", duration_myFunc_rev);

    printf("myData[21] hw checksum: %u\n", hw_rev_CalculatedCRC2);
    printf("myData[21] hw duration: %uus\n", duration_hw_rev2);

    printf("myData[21] myFunc checksum: %u\n", myCalculatedCRC_rev2);
    printf("myData[21] myFunc duration: %uus\n", duration_myFunc_rev2);

    /* Pause for the debugger */
    __no_operation();
}
//![Simple CRC32 Example] 

/* Standard software calculation of CRC32 */
static uint32_t calculateCRC32(uint8_t* data, uint32_t length)
{
    uint32_t ii, jj, byte, crc, mask;

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

static uint32_t compute_simple_checksum(uint8_t* data, uint32_t length) {
    /*
     * Function will:
     * - add all data in array together into a 32-bit word
     * - returns a reversed sum value, i.e., 1 -> 0 or 0 -> 1
     */

    uint32_t result = 0x0;

    int i;

    // iterate through data array and add all
    for (i=0; i<length; i++) {
        result = result + data[i];
    }

    // complement operator reverses bits
    return ~result;
}

