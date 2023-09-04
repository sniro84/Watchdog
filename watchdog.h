/************************************************ 
Exercise      : WatchDog
Implmented by : Snir Holland
Reviewed by   : 
Date          : 22/7/2023
File          : watchdog.c
*************************************************/

#ifndef __ILRD_WATCHDOG__
#define __ILRD_WATCHDOG__

#include <stddef.h> /*size_t*/

/**
 * WDStart
 * Description: 
 *      Start a watchdog to protect a section of code.
 * Arguments:
 *      path: path of executable file
 * Return: 
 *      status of watchdog operation     
*/
int WDStart(char **file_path);

/**
 * WDStop
 * Description: 
 *      Stop the watchdog.
 * Arguments:
 *      timeout: max time to wait for graceful exit    
*/
void WDStop(size_t timeout);

#endif /* __ILRD_WATCHDOG__*/
