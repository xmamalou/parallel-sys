#include "exercises.h"

#include <stdio.h>

void pi_calc(bool do_serial) 
{
    if(do_serial)
    {
        printf("Hello pi -- serial version!\n");
    } else {
        printf("Hello pi -- parallel version!\n");
    }
}