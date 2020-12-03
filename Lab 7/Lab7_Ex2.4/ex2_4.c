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

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#define TASKSTACKSIZE   2048

Void task1(UArg arg0, UArg arg1);
Void task2(UArg arg0, UArg arg1);
Void task3(UArg arg0, UArg arg1);

Task_Struct task1Struct, task2Struct, task3Struct;
Char task1Stack[TASKSTACKSIZE], task2Stack[TASKSTACKSIZE], task3Stack[TASKSTACKSIZE];

UART_Handle uart;
UART_Params uartParams;

int main()
{
    /* Construct BIOS objects */
    Task_Params taskParams;

    /* Call driver init functions */
    Board_init();

    UART_init();    // Driver init

    // Set up communication parameters and open the device
    UART_Params_init(&uartParams);
    uartParams.readEcho = UART_ECHO_ON;
    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL) {
        printf("Failed to open UART.\n");
        while(1);
    }

    /* Construct task threads */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;

    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)task1, &taskParams, NULL);

    taskParams.stack = &task2Stack;
    Task_construct(&task2Struct, (Task_FuncPtr)task2, &taskParams, NULL);

    taskParams.stack = &task3Stack;
    Task_construct(&task3Struct, (Task_FuncPtr)task3, &taskParams, NULL);

    BIOS_start();    /* Does not return */
    return(0);
}

Void task1(UArg arg0, UArg arg1)
{
    printf("Task1\n");

    // setup button
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    // prevStatus keeps track of previous state
    // time used to keep track of how long since last state change
    uint32_t status, prevStatus = 1;
    int time = 0;
    char buffer[10];

    while (1) {
        status = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);
        sprintf(buffer, "S1%u\n", status);
        if (status != prevStatus || time >= 50) {
            UART_write(uart, buffer, strlen(buffer));
            time = 0;
        }

        // set prevStatus to current status
        prevStatus = status;

        // increment time
        time++;

        Task_sleep(100);
    }
}

Void task2(UArg arg0, UArg arg1)
{
    printf("Task2\n");

    // setup button
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN4);

    // prevStatus keeps track of previous state
    // time used to keep track of how long since last state change
    uint32_t status, prevStatus = 1;
    int time = 0;
    char buffer[10];

    while (1) {
        status = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4);
        sprintf(buffer, "S2%u\n", status);
        if (status != prevStatus || time >= 50) {
            UART_write(uart, buffer, strlen(buffer));
            time = 0;
        }

        // set prevStatus to current status
        prevStatus = status;

        // increment time
        time++;

        Task_sleep(100);
    }
}

Void task3(UArg arg0, UArg arg1)
{
    // setup LED
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    uint8_t UARTval;

    while (1) {
        // read UART state
        UART_read(uart, &UARTval, 1);
        printf("%u", UARTval);

        // toggle LED
        if(UARTval == 49) {
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        } else if (UARTval == 48) {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }

        Task_sleep(100);
    }
}
