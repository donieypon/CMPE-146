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

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);

Task_Struct task1Struct, task2Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];

int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call driver init functions */
    Board_init();

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    BIOS_start();    /* Does not return */
    return(0);
}

// blink red
Void task1(UArg arg0, UArg arg1)
{
    // setup red LED
    /* Configuring P2.0 outputs */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // red

    /* Bring LED to low */
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // red

    while (1) {

        // blink LED
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

        // delay for 1000ms = 1 Hz
        Task_sleep(1000);
    }
}

// blink green
Void task2(UArg arg0, UArg arg1)
{
    // setup green LED
    /* Configuring P2.0 outputs */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1); // green

    /* Bring LED to low */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1); // green

    while (1) {

        // blink LED
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);

        // delay for 4000ms = 0.25 Hz
        Task_sleep(4000);
    }
}
