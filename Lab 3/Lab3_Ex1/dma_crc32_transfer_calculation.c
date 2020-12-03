/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Statics */
static volatile uint32_t crcSignature;

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

/* Extern */
//extern uint8_t data_array[];

#define CRC32_SEED 0xFFFFFFFF

uint8_t data_array[1024];

int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    // produce CRC-32 checksum using hardware method
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
    int ii;
    for (ii = 0; ii < sizeof(data_array); ii++) {
        MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
    }
    uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);
    printf("hwCRC = %08x\n", hwCRC);

    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    /* Setting Control Indexes. In this case we will set the source of the
     * DMA transfer to our random data array and the destination to the
     * CRC32 data in register address*/
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
            UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
            UDMA_MODE_AUTO, data_array,
            (void*) (&CRC32->DI32), 1024);

    /* Assigning/Enabling Interrupts */
    MAP_DMA_assignInterrupt(DMA_INT1, 0);

    // enables DMA interrupt
    MAP_Interrupt_enableInterrupt(INT_DMA_INT1);

    // allows processor to respond to interrupts
    MAP_Interrupt_enableMaster();
    
    /* Enabling DMA Channel 0 */
    MAP_DMA_enableChannel(0);

    // initialize result register in CRC-32 accelerator
    MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);

    /* Forcing a software transfer on DMA Channel 0 */
    MAP_DMA_requestSoftwareTransfer(0);

    while(1)
    {
        MAP_PCM_gotoLPM0();
    }

}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);
    printf("dmaCRC = %08x\n", crcSignature);
}
