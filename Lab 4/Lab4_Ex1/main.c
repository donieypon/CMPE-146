/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char* string1 = "12345";
char string2[] = "12345";
//char string3 = "12345";

int main(void)
{

    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    //printf("string3: %s\n", string3);

    string1[0] = '0';
    string2[0] = '0';
    // string3[0] = '0'; <- not possible

    printf("string1: %s\n", string1);
    printf("string2: %s\n", string2);
    //printf("string3: %s\n", string3);

}
