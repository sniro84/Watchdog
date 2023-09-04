#include <stdio.h>

#include "watchdog.h"

int main(int argc, char **argv)
{
    printf("wd_exec.out started...\n");

    WDStart(&argv[0]);    
    
    printf("wd_exec.out stopped...\n");

    return (0);
}