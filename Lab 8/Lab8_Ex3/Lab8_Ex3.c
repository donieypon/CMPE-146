#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/drivers/Board.h>
#define __MSP432P4XX__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define TASKSTACKSIZE   2048

Void task_timing(UArg arg0, UArg arg1);
Void task_LEDcontrol(UArg arg0, UArg arg1);

// Ex 2
#define NTASKS 4
Task_Struct taskStructs[NTASKS];
Char taskStacks[NTASKS][TASKSTACKSIZE];

Semaphore_Struct semaStruct;
Semaphore_Handle semaphoreHandle = NULL;

int main()
{
    // Init drivers
    Board_init();

    srand(time(NULL));      // Set seed for random number generator

    // Construct tasks
    Task_Params timingParams, redParams, greenParams, blueParams;

    Task_Params_init(&timingParams);
    Task_Params_init(&redParams);
    Task_Params_init(&greenParams);
    Task_Params_init(&blueParams);

    timingParams.stackSize = TASKSTACKSIZE;
    redParams.stackSize = TASKSTACKSIZE;
    greenParams.stackSize = TASKSTACKSIZE;
    blueParams.stackSize = TASKSTACKSIZE;

    // task 1 - blinking task
    timingParams.stack = &taskStacks[0];
    Task_construct(&taskStructs[0], (Task_FuncPtr)task_timing, &timingParams, NULL);

    // task 2 - Red LED
    redParams.arg0 = GPIO_PORT_P1;
    redParams.arg1 = GPIO_PIN0;
    redParams.stack = &taskStacks[1];
    Task_construct(&taskStructs[1], (Task_FuncPtr)task_LEDcontrol, &redParams, NULL);

    // task 3 - Green LED
    greenParams.arg0 = GPIO_PORT_P2;
    greenParams.arg1 = GPIO_PIN1;
    greenParams.stack = &taskStacks[2];
    Task_construct(&taskStructs[2], (Task_FuncPtr)task_LEDcontrol, &greenParams, NULL);

    // task 4 - Blue LED
    blueParams.arg0 = GPIO_PORT_P2;
    blueParams.arg1 = GPIO_PIN2;
    blueParams.stack = &taskStacks[3];
    Task_construct(&taskStructs[3], (Task_FuncPtr)task_LEDcontrol, &blueParams, NULL);

    // Construct a semaphore object
    Semaphore_Params semaParams;
    Semaphore_Params_init(&semaParams);                 // Initialize structure with default parameters
    Semaphore_construct(&semaStruct, 0, &semaParams);   // Create an instance of semaphore object
    semaphoreHandle = Semaphore_handle(&semaStruct);

    BIOS_start();    // Jump to the OS and won't return
    return(0);
}

Void task_timing(UArg arg0, UArg arg1) {
    // control timing
    while (1) {
        // random time freq between 0.2 Hz to 1 Hz
        // **between 5000 ms (0.2 Hz) to 1000 ms (1 Hz)
        int ms = (rand() % (5000 + 1 - 1000) + 1000) / 2;

        uint32_t i;
        for (i = 0; i < NTASKS - 1; i++) {
            Semaphore_post(semaphoreHandle);
        }

        Task_sleep(ms);
    }
}

Void task_LEDcontrol(UArg arg0, UArg arg1)
{
    // setup LED
    MAP_GPIO_setAsOutputPin(arg0, arg1);
    MAP_GPIO_setOutputLowOnPin(arg0, arg1);

    while(1)
    {
        Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER);

        // on or off
        MAP_GPIO_toggleOutputOnPin(arg0, arg1);

        // suspend itself
        Task_yield();
    }
}
