/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


void delay_ms(uint32_t count)
{
    uint32_t t0 = MAP_Timer32_getValue(TIMER32_0_BASE);
    uint32_t clock_rate = MAP_CS_getMCLK();
    uint32_t t_final = t0 - (uint32_t)((unsigned long long)clock_rate * count / 1000);
    while (1)
        if (MAP_Timer32_getValue(TIMER32_0_BASE) < t_final)
            break;
}

int main(void)
{
    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    // setup timer
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
                           TIMER32_FREE_RUN_MODE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, 0);

    uint32_t freq;

    // show current system clock frequency
    freq = MAP_CS_getMCLK();
    printf("MCLK: %uHz\n", freq);

    // setup control register of capacitive touch feature
    CAPTIO0CTL = 0;
    CAPTIO0CTL |= (1 << 8); // Enable CAPTIO
    CAPTIO0CTL |= 0b0100 << 4; // Choose Port 4
    CAPTIO0CTL |= 0b0001 << 1; // Choose Pin 1

    // setup counter to count incoming pulses
    Timer_A_ContinuousModeConfig timer_continuous_obj;
    timer_continuous_obj.clockSource = TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK; // refers to INCLK input clock
    timer_continuous_obj.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1; // divide by 1 to measure with high precision
    timer_continuous_obj.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    timer_continuous_obj.timerClear = TIMER_A_DO_CLEAR;
    MAP_Timer_A_configureContinuousMode(TIMER_A2_BASE, &timer_continuous_obj);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);

    uint32_t counterValue;
    uint32_t oscillatorFreq;

    /*
     * F = 1 / T
     * ex: 2ms, 1000 pulses
     *  --> T = 2ms / 1000
     *  --> F = 1 / T = 1 / (2ms / 1000) = 1000 / 2ms
     *  so, F = counterValue / 2ms delay
     */

    // loop to continually show state of oscillator
    while (1) {
        // clear counter
        MAP_Timer_A_clearTimer(TIMER_A2_BASE);

        // delay for 2ms
        delay_ms(2);

        // read counter register
        counterValue = MAP_Timer_A_getCounterValue(TIMER_A2_BASE);

        // convert counterValue to frequency
        oscillatorFreq = (uint32_t)(counterValue / 0.002);

        printf("counterValue = %u\n", counterValue);
        printf("osciallatorFreq = %uHz\n", oscillatorFreq);

        // delay for 500ms
        delay_ms(500);
    }
}
