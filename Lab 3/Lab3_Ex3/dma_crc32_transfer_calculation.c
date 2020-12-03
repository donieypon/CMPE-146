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

#define CRC32_SEED 0xFFFFFFFF

uint8_t data_array[1024];

// flag for ISR and main() to communicate
volatile int dma_done;

// ex 3
int size_array[] = {2, 4, 16, 32, 64, 128, 256, 786, 1024};

int main(void)
{
    /* Halting Watchdog */
    MAP_WDT_A_holdTimer();

    // initialize timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    // setup DMA externally from for loop
    /* Configuring DMA module */
    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    int jj;

    for (jj = 0; jj < 9; jj++) {
        /*  produce CRC-32 checksum using hardware method  */

        int block_size = size_array[jj];

        // print block size:
        printf("Block size = %i\n", block_size);

        // start
        uint32_t t0_hw = MAP_Timer32_getValue(TIMER32_0_BASE);

        MAP_CRC32_setSeed(CRC32_SEED, CRC32_MODE);
        int ii;
        for (ii = 0; ii < block_size; ii++) {
            MAP_CRC32_set8BitData(data_array[ii], CRC32_MODE);
        }
        uint32_t hwCRC = MAP_CRC32_getResult(CRC32_MODE);

        // finish
        uint32_t t1_hw = MAP_Timer32_getValue(TIMER32_0_BASE);
        uint32_t duration_hw = (t0_hw-t1_hw) / 3;

        printf("hwCRC = %08x\n", hwCRC);
        printf("hw duration = %uus\n", duration_hw);

        /* ------------------------------------------------ */

        // start DMA
        uint32_t t0_DMA = MAP_Timer32_getValue(TIMER32_0_BASE);

        // before transfer, set dma_done flag to 0
        dma_done = 0;

        /* Setting Control Indexes. In this case we will set the source of the
         * DMA transfer to our random data array and the destination to the
         * CRC32 data in register address*/
        MAP_DMA_setChannelControl(UDMA_PRI_SELECT,
                                  UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1024);
        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT,
                                   UDMA_MODE_AUTO, data_array,
                                   (void*) (&CRC32->DI32), block_size);

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
            if (dma_done == 1) {
                break;
            }
        }

        // finish DMA
        uint32_t t1_DMA = MAP_Timer32_getValue(TIMER32_0_BASE);
        uint32_t duration_DMA = (t0_DMA-t1_DMA) / 3;

        // print checksum from DMA
        printf("DMA crcSignature = %08x\n", crcSignature);
        printf("DMA duration = %uus\n", duration_DMA);


        // calculate speedup
        uint32_t speedup_DMA = duration_hw / duration_DMA;
        uint32_t speedup_hw = duration_DMA / duration_hw;

        if (speedup_DMA > 0 && duration_hw > duration_DMA) {
            printf("Speedup of DMA over hw = ~%ux faster\n", speedup_DMA);
        } else if (speedup_hw > 0 && duration_DMA > duration_hw) {
            printf("Speedup of hw over DMA = ~%ux faster\n", speedup_hw);
        }
        printf("-----------------------------------------------\n");
    }

}

/* Completion interrupt for DMA */
void DMA_INT1_IRQHandler(void)
{
    MAP_DMA_disableChannel(0);
    crcSignature = MAP_CRC32_getResult(CRC32_MODE);

    // indicate completion of transfer
    dma_done = 1;
}
