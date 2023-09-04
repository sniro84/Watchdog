#include <stdio.h>
#include <unistd.h>

#include "watchdog.h"

int main(int argc, char **argv)
{
    int i = 0;
    printf("user_app.out started...\n");
    
    WDStart(argv);

    for(i=0; i<5; ++i)
    {
        printf("Client is still running...\n");
        sleep(1);
    }

    WDStop(5);

    printf("user_app.out stopped...\n");

    return (0);
}