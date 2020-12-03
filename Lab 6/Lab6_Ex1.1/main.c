#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdlib.h>

#define CRC32_POLY              0xEDB88320
#define CRC32_INIT              0xFFFFFFFF

/* DMA Control Table */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(controlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#elif defined(__CC_ARM)
__align(1024)
#endif
uint8_t controlTable[1024];

#define DATA_SIZE 1024              // Cannot be greater than 1024 for DMA method
uint8_t myData[DATA_SIZE];
static uint32_t calculateCRC32(uint8_t* data, uint32_t length);

volatile uint32_t hwCalculatedCRC, swCalculatedCRC, dmaCalculatedCRC;

#define LOOP_COUNT 500            // Number of iterations to do
//#define DO_PRINTF

//#define USE_SW_METHOD
//#define USE_HW_METHOD
//#define USE_DMA_METHOD
#define USE_DMA_LPM_METHOD

int dma_channel_number = 0;
volatile bool dma_done;

int main(void)
{
    int loop;
    /* Stop WDT */
    MAP_WDT_A_holdTimer();
#ifdef DO_PRINTF
    printf("Lab 6. Exercise 1.\n");
#endif

#if defined(USE_DMA_METHOD) || defined(USE_DMA_LPM_METHOD)
    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);

    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);
    MAP_Interrupt_enableMaster();

    for (loop=0; loop<LOOP_COUNT; loop++)
    {
        MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT, UDMA_MODE_AUTO, (void*)myData, (void*) (&CRC32->DI32), sizeof(myData));

        /* Enabling DMA Channel 0 */
        MAP_DMA_enableChannel(0);

        /* Forcing a software-trigger transfer on DMA Channel 0 */
        dma_done = 0;
        MAP_DMA_requestSoftwareTransfer(0);

        while(!dma_done)
        {
            #ifdef USE_DMA_LPM_METHOD
            MAP_PCM_gotoLPM0();
            #endif
        }
        dmaCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
        #ifdef DO_PRINTF
        printf("dmaCalculatedCRC=%x\n", dmaCalculatedCRC);
        #endif
    }
#endif

#ifdef USE_HW_METHOD
    for (loop=0; loop<LOOP_COUNT; loop++)
    {
        MAP_CRC32_setSeed(CRC32_INIT, CRC32_MODE);
        uint32_t ii;
        for (ii = 0; ii < sizeof(myData); ii++)
            MAP_CRC32_set8BitData(myData[ii], CRC32_MODE);

        /* Getting the result from the hardware module */
        hwCalculatedCRC = MAP_CRC32_getResultReversed(CRC32_MODE) ^ 0xFFFFFFFF;
        #ifdef DO_PRINTF
        printf("hwCalculatedCRC=%x\n", hwCalculatedCRC);
        #endif
    }
#endif

#ifdef USE_SW_METHOD
    for (loop=0; loop<LOOP_COUNT; loop++)
    {
        /* Calculating the CRC32 checksum through software */
        swCalculatedCRC = calculateCRC32((uint8_t*) myData, sizeof(myData));
        #ifdef DO_PRINTF
        printf("swCalculatedCRC=%x\n", swCalculatedCRC);
        #endif
    }
#endif
    #ifdef DO_PRINTF
    printf("Done.\n");
    #endif
    abort();        // Stop program execution immediately
}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    dma_done = 1;
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
