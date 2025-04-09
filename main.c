#include <omp.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(
        int argc, char* argv[]) 
{
    if(argc > 1 && strcmp(argv[1], "--farts") == 0) 
    {
        printf("Farts to you too!\n");
    }

    printf("Hello World!");

    return 0;
}