/************************************************ 
EXERCISE      : DS#9: ILRD_UID
Implmented by : Snir Holland
Reviewed by   : Yehuda Tarona
Date          : 16/5/2023
File          : uid.c
*************************************************/

#include "uid.h"

#include <unistd.h>     /* getpid */
#include <netinet/in.h> /* inet_ntoa */
#include <arpa/inet.h>  /* inet_ntoa */
#include <sys/socket.h> /* struct sockaddr_in */
#include <ifaddrs.h>    /* getifaddrs */
#include <time.h>       /* time */
#include <string.h>     /* strcpy */
#include <assert.h>     /* assert */
#include <pthread.h>    /* mutex */
#include <stdio.h>      /* fprintf */
#include <stdlib.h>     /* EXIT_FAILURE */

const ilrd_uid_t UIDBadUID = {0};

enum status {SAME = 1 ,DIFFERENT = 0};

#define MUTEX_SUCCESS   (0)   

/* This function gets a buffer and insert an IP address into it */
static unsigned char *GetIPAddress(unsigned char *addr);

ilrd_uid_t UIDCreate(void)
{
	static size_t counter = 0;

    pthread_mutex_t mutex = {0};
	
	ilrd_uid_t new_uid = {0};
	
    if (MUTEX_SUCCESS != pthread_mutex_init(&mutex, NULL))
    {
        fprintf(stderr, "Failed to create mutex.\n");
        exit(EXIT_FAILURE);
    }

	if (NULL == GetIPAddress(new_uid.ip))
	{
		return UIDBadUID;
	}

    pthread_mutex_lock(&mutex);
    new_uid.counter = counter;
    ++counter;
    pthread_mutex_unlock(&mutex);
    
	new_uid.time = time(NULL);    
    new_uid.pid = (size_t)getpid();
    
    if (MUTEX_SUCCESS != pthread_mutex_destroy(&mutex))
    {
        fprintf(stderr, "Failed to destroy mutex.\n");
        exit(EXIT_FAILURE);
    }

    return new_uid;
}

int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
	if (uid1.counter != uid2.counter || uid1.time != uid2.time || uid1.pid != uid2.pid)
	{
		return (DIFFERENT);
	}
	
	if (0 != strcmp( (char*)uid1.ip , (char*)uid2.ip))
	{
		return (DIFFERENT);
	}
	
	return (SAME);
}

/* This helper function loads an ip address into the UID */
static unsigned char *GetIPAddress(unsigned char *addr)
{
	struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    
    assert(NULL != addr);
    
    if (-1 == getifaddrs(&ifap))
    {
    	return NULL;
    }

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) 
        {
            sa = (struct sockaddr_in *)ifa->ifa_addr;
            strcpy( (char*)addr , inet_ntoa(sa->sin_addr) );
  			break;
        }
    }

    freeifaddrs(ifap);
	
    return addr;
}


