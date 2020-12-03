/* Lab 2
 *
 *
 *
 *
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//![Simple GPIO Config]
int main(void)
{
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
        /* Delay Loop */
        for(ii=0;ii<50000;ii++)
        {
        }

        // blinks LED
        if (counter % 2 == 0) {
            *alias_addr = 0x0;
        } else {
            *alias_addr = 0x1;
        }
        counter++;

    }
}
//![Simple GPIO Config]


