
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

#define TIMER_MAX 0x000fffff

int main(void)
{

    // setup timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
    TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    uint32_t delayTimes[] = {10000, 5000, 2000, 1000, 5, 1, 0, 10000};
    uint32_t t1, duration;

    // show current system clock frequency
    freq = MAP_CS_getMCLK();
    printf("MCLK: %fHz\n", freq);

    uint32_t i = 0;
    for (i = 0; i < 8; i++) {

        t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
        delay_ms(delayTimes[i]);
        t1 = MAP_Timer32_getValue(TIMER32_0_BASE);

        duration = (t0 - t1) * (1000000 / freq);

        printf("Timer delay: %uus\n", delayTimes[i] * 1000);
        printf("Measured duration: %uus\n", duration);
        printf("Time difference: %uus\n", duration - (delayTimes[i] * 1000));
        printf("-------------------------------\n");
    }

}

// modify to handle wrap around
void delay_ms(uint32_t count) {
    // get delay
    uint32_t delay = count * (freq / 1000); // in us

    // reference to caller timer
    uint32_t globalTimer = t0;

    // variables for comparison
    uint32_t currTime = 0;
    uint32_t prevTime = 0;

    while (1) {
        // get current timer value
        currTime = MAP_Timer32_getValue(TIMER32_0_BASE) & TIMER_MAX;

        // check if current time is greater than previous time (wraparound)
        if (currTime > prevTime) {
            globalTimer = globalTimer - prevTime - (TIMER_MAX - currTime);
        } else {
            globalTimer = globalTimer - (prevTime - currTime);
        }

        // set previous time to current time
        prevTime = currTime;

        // check if delay has been satisfied
        if (delay <= (t0 - globalTimer)) {
            break;
        }
    }

}
