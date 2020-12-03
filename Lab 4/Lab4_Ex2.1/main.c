
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

    int delayTimes[] = {5000, 2000, 1000, 5, 1, 0};
    float time;
    uint32_t t1, duration;

    // show current system clock frequency
    freq = MAP_CS_getMCLK();
    printf("MCLK: %fHz\n", freq);

    uint32_t i = 0;
    for (i = 0; i < 6; i++) {

        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(delayTimes[i]);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        duration = t0 - t1;
        time = duration * (1000000 / freq);

        printf("Timer delay: %ius\n", delayTimes[i] * 1000);
        printf("Measured duration: %.0fus\n", time);
        printf("Time difference: %.0fus\n", time - (delayTimes[i] * 1000));
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
