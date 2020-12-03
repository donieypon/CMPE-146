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

Semaphore_Struct semaStruct;
Semaphore_Handle semaphoreHandle = NULL;

int main()
{
    // Init drivers
    Board_init();

    srand(time(NULL));      // Set seed for random number generator

    // Construct tasks
    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;

    // Task 1
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    // Task 2
    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    // Construct a semaphore object
    Semaphore_Params semaParams;
    Semaphore_Params_init(&semaParams);                 // Initialize structure with default parameters
    Semaphore_construct(&semaStruct, 0, &semaParams);   // Create an instance of semaphore object
    semaphoreHandle = Semaphore_handle(&semaStruct);

    BIOS_start();    // Jump to the OS and won't return
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{
    Semaphore_pend(semaphoreHandle, BIOS_WAIT_FOREVER);
    printf("Task1\n");
}

Void task2(UArg arg0, UArg arg1)
{
    printf("Task2\n");
    Semaphore_post(semaphoreHandle);
}