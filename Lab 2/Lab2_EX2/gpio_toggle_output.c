/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//![Simple GPIO Config]
int main(void)
{
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);
    printf("%u\n", MAP_CS_getMCLK());

    volatile uint32_t ii;

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    /* Configuring P2.0, P2.1, P2.2 as outputs */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); // red
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); // green
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2); // blue

    /* Bring LEDs to low */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); // red
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1); // green
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2); // blue

    uint8_t *alias_addr;
    unsigned int port_addr = 0x40004C03;
    unsigned int peripheral_region_addr = 0x40000000;
    unsigned int bit_pos = 0x2;
    unsigned int alias_region_addr = 0x42000000;

    alias_addr = (port_addr - peripheral_region_addr) * 32 + bit_pos * 4 + alias_region_addr;

    int counter = 0;

    while (1)
    {
        // read counter value at beginning
        uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        printf("counter value (1): %u\n", t0);

        /* Delay Loop */
        for(ii=0;ii<100000;ii++)
        {
        }

        // blinks LED
        if (counter % 2 == 0) {
            *alias_addr = 0x0; // off
        } else {
            *alias_addr = 0x1; // on
        }
        counter++;

        // read counter value at end
        uint32_t t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        // print counter value
        printf("counter value (2): %u\n", t1);

        // calculate duration
        uint32_t duration = (t0-t1) * 0.000000333 * 1000;

        // print duration to console
        printf("time duration: %ums\n", duration);

    }
}
//![Simple GPIO Config]


