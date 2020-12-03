
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// global variables
uint32_t t0;
float freq;

void delay_ms(uint32_t count);

int main(void)
{

    // setup timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
                           TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    float time;
    float delay = 1000;
    uint32_t t1, duration;

    // show current system clock frequency
    freq = MAP_CS_getMCLK();
    printf("MCLK: %fHz\n", freq);

    /* Configuring P2.0 outputs */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); // red

    /* Bring LED to low */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); // red

    while (1) {

        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);

        // blink LED
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);

        delay_ms(delay);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        duration = t0 - t1;
        time = duration * (1000 / freq);

        printf("Timer delay: %fms\n", delay);
        printf("Measured duration: %fms\n", time);
        printf("Time difference: %fms\n", time - delay);
        printf("-------------------------------\n");
    }

}

void delay_ms(uint32_t count) {
    // get delay
    uint32_t delay = count * (freq / 1000); // in us

    // compare with global variable
    while (1) {
        if (delay <= t0 - MAP_Timer32_getValue(TIMER32_0_BASE)) {
            break;
        }
    }

}
