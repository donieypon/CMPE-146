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

//Void task1(UArg arg0, UArg arg1);
//Void task2(UArg arg0, UArg arg1);
Void taski(UArg arg0, UArg arg1);

//Task_Struct task1Struct, task2Struct;
//Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE];

// Ex 2
#define NTASKS 3
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
    Task_Params taskParams1, taskParams2, taskParams3;

    Task_Params_init(&taskParams1);
    Task_Params_init(&taskParams2);
    Task_Params_init(&taskParams3);

    taskParams1.stackSize = TASKSTACKSIZE;
    taskParams2.stackSize = TASKSTACKSIZE;
    taskParams3.stackSize = TASKSTACKSIZE;

    // create 3 tasks
    // task 1
    taskParams1.arg0 = 1;
    taskParams1.arg1 = 1;
    taskParams1.stack = &taskStacks[0];
    Task_construct(&taskStructs[0], (Task_FuncPtr)taski, &taskParams1, NULL);

    // task 2
    taskParams2.arg0 = 2;
    taskParams2.arg1 = 2;
    taskParams2.stack = &taskStacks[1];
    Task_construct(&taskStructs[1], (Task_FuncPtr)taski, &taskParams2, NULL);

    // task 3
    taskParams3.arg0 = 3;
    taskParams3.arg1 = 3;
    taskParams3.stack = &taskStacks[2];
    Task_construct(&taskStructs[2], (Task_FuncPtr)taski, &taskParams3, NULL);

    // Construct a semaphore object
    Semaphore_Params semaParams;
    Semaphore_Params_init(&semaParams);                 // Initialize structure with default parameters
    Semaphore_construct(&semaStruct, 0, &semaParams);   // Create an instance of semaphore object
    semaphoreHandle = Semaphore_handle(&semaStruct);

    BIOS_start();    // Jump to the OS and won't return
    return(0);
}

Void taski(UArg arg0, UArg arg1)
{
    while(1)
    {
        printf("Task: arg0=%u, arg1=%u\n", (uint32_t)arg0, (uint32_t)arg1);
        Task_sleep(1000);
    }
}
