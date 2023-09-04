/************************************************
Exercise      : WatchDog
Implmented by : Snir Holland
Reviewed by   :
Date          : 29/7/2023
File          : watchdog.c
*************************************************/

#define _XOPEN_SOURCE   /* sigset_t */
#define _POSIX_SOURCE   /* sigaction */
#define _DEFAULT_SOURCE /* cancel unsetenv warning */
#include <stdlib.h>     /* fprintf */
#include <stdio.h>      /* printf */
#include <signal.h>
#include <stdatomic.h> /* atomic_int */
#include <semaphore.h> /* sem_t, semaphore functions */
#include <fcntl.h>     /* O_CREAT*/
#include <unistd.h>    /* getpid, getppid*/
#include <pthread.h>   /* pthread_create, pthread_join */

#include "scheduler.h"

#define TASK1_DELAY (0)
#define TASK1_INTERVAL (1)
#define TASK2_DELAY (0)
#define TASK2_INTERVAL (5)
#define TASK3_DELAY (0)
#define TASK3_INTERVAL (1)

#define SEM_PERMISSIONS (0644)
#define SEM_NAME ("/wd_sem")
#define SEM_MODE (O_CREAT)
#define SEM_INITIAL_VALUE (0)

#define PID_BUFFSIZE (10)
#define CHILD (0)
#define WATCHDOG_PATH ("./wd_exec.out")

enum wd_status
{
    WD_SUCCESS,
    WD_FAILURE
};

enum dnr_status
{
    FALSE,
    TRUE
};

/* -------------- Global variables ----------------- */
sched_t *sched = NULL;
sem_t *semaphore = NULL;
atomic_int life_count = 0;
int dnr_wd = FALSE;
pid_t monitored_pid = 0;
pthread_t scheduler_thread = 0;

/* -------------- Tasks ----------------- */
static int TaskIncrementLifeCount(void *param);
static int TaskCheckLifeCount(void *param);
static int TaskCheckDnrStatus(void *param);

/* -------------- Signal Handlers ----------------- */
static void HandlerSIGUSR1(int sig, siginfo_t *sig_info, void *ucontext);
static void HandlerSIGUSR2(int sig, siginfo_t *sig_info, void *ucontext);

/* -------------- Static functions ----------------- */
static int InitSched(char **file_path);
static void InitHandlers();
static void InitSemaphore();
static void *RunSched(void *param);
static void DummyClean(void *param);
static void SetWDEnvVar();
static pid_t GetPidFromEnv();
static int IsRunningProcessWatchdog();
static int IsWatchdogActive();
static int Revive(char **file_path);

/* -------------- API ----------------- */
int WDStart(char **file_path)
{
    sigset_t set = {0};

    printf("---- Process #%d Started -----\n", getpid());
    printf("Initializing...\n");

    InitHandlers();
    InitSched(file_path);
    InitSemaphore();

    if (!IsWatchdogActive())
    {
        monitored_pid = fork();

        if (CHILD == monitored_pid)
        {
            printf("Executing watchdog...\n");

            SetWDEnvVar();
            execl(WATCHDOG_PATH, *file_path);
        }

        printf("waiting for watchdog initialization to finish...\n");

        sem_wait(semaphore);

        printf("watchdog initialization finished!! starting thread...\n");

        pthread_create(&scheduler_thread, NULL, &RunSched, NULL);
    }

    else if (IsRunningProcessWatchdog())
    {
        monitored_pid = getppid();

        printf("I am watchdog with pid: %d.\n", getpid());
        printf("My parent (user) has pid: %d.\n", getppid());
        printf("SemPost is taking place.\n");

        sem_post(semaphore);

        RunSched(sched);
    }

    else
    {
        monitored_pid = GetPidFromEnv();
    }

    sigemptyset(&set);

    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    sigprocmask(SIG_BLOCK, &set, NULL);

    return WD_SUCCESS;
}

void WDStop(size_t timeout)
{
    sigset_t set = {0};

    printf("Stopping watchdog...\n");

    while (timeout > 0)
    {
        timeout = sleep(timeout);
    }

    /* Send SIGUSR2*/
    kill(monitored_pid, SIGUSR2);

    sigfillset(&set);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

/* -------------- InitSched ----------------- */
static int InitSched(char **file_path)
{
    printf("Initializing schedule...\n");

    sched = SchedCreate();
    if (NULL == sched)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        return WD_FAILURE;
    }

    SchedAdd(sched, TASK1_DELAY, TASK1_INTERVAL, &TaskIncrementLifeCount, NULL, NULL, &DummyClean);
    SchedAdd(sched, TASK2_DELAY, TASK2_INTERVAL, &TaskCheckLifeCount, (void *)file_path, NULL, &DummyClean);
    if (IsRunningProcessWatchdog())
    {
        printf("Adding third task to wd scheduler...\n");
        SchedAdd(sched, TASK3_DELAY, TASK3_INTERVAL, &TaskCheckDnrStatus, NULL, NULL, &DummyClean);
    }

    return SUCCESS;
}

static void HandlerSIGUSR1(int sig, siginfo_t *sig_info, void *ucontext)
{
    printf("SIGUSR1 Recieved!\n");
    printf("Incrementing life count of other process %d.\n", monitored_pid);
    atomic_fetch_add(&life_count, 1);
    printf("life counter = %d\n", life_count);
}

static void HandlerSIGUSR2(int sig, siginfo_t *sig_info, void *ucontext)
{
    printf("SIGUSR2 Recieved!\n");
    printf("DNR flag has been turned on.\n");
    dnr_wd = TRUE;
}

static int TaskIncrementLifeCount(void *param)
{
    /* Task1: send SIGUSR1 */
    printf("Sending SIGUSR1 to other process.\n");

    kill(monitored_pid, SIGUSR1);

    return OP_CONTINUE;
}

static int TaskCheckLifeCount(void *param)
{
    /* Task2: check friend's counter */
    char **file_path = (char **)param;

    printf("Checking counter of other process.\n");

    if (0 != life_count)
    {
        printf("Life signal recieved, resetting count.\n");
        atomic_fetch_xor(&life_count, life_count);
        printf("life counter = %d\n", life_count);
    }
    else
    {
        printf("Reviving...\n");
        Revive(file_path);
    }

    return OP_CONTINUE;
}

static int TaskCheckDnrStatus(void *param)
{
    /* Task3: check dnr_wd flag */

    printf("Checking DNR flag.\n");

    if (dnr_wd)
    {
        pthread_join(scheduler_thread, NULL);

        SchedClear(sched);
        SchedStop(sched);
        SchedDestroy(sched);

        unsetenv("WD_PID");

        sem_destroy(semaphore);

        return OP_DONE;
    }

    return OP_CONTINUE;
}

static void DummyClean(void *param)
{
    (void)param;
}

static void InitHandlers()
{
    struct sigaction sigusr1_action = {0};
    struct sigaction sigusr2_action = {0};

    printf("Initializing handlers.\n");

    sigusr1_action.sa_flags = SA_SIGINFO;
    sigusr2_action.sa_flags = SA_SIGINFO;

    sigusr1_action.sa_sigaction = &HandlerSIGUSR1;
    sigusr2_action.sa_sigaction = &HandlerSIGUSR2;

    sigaction(SIGUSR1, &sigusr1_action, NULL);
    sigaction(SIGUSR2, &sigusr2_action, NULL);
}

static void InitSemaphore()
{
    printf("Initializing semaphore.\n");

    semaphore = sem_open(SEM_NAME, SEM_MODE, SEM_PERMISSIONS, SEM_INITIAL_VALUE);

    /* unlink prevents the semaphore existing forever
       if a crash occurs during the execution         */
    sem_unlink(SEM_NAME);
}

static void SetWDEnvVar()
{
    char wd_env[PID_BUFFSIZE] = {0};
    sprintf(wd_env, "%d", getpid());
    setenv("WD_PID", wd_env, 0);
}

static pid_t GetPidFromEnv()
{
    char *pid_str = getenv("WD_PID");
    if (NULL == pid_str)
    {
        return (-1);
    }

    return atoi(pid_str);
}

static int IsWatchdogActive()
{
    return (NULL != getenv("WD_PID"));
}

static int IsRunningProcessWatchdog()
{
    return (IsWatchdogActive() && getpid() == GetPidFromEnv());
}

static void *RunSched(void *param)
{
    printf("%d is running scheduler...\n", getpid());
    SchedRun(sched);

    return NULL;
}

static int Revive(char **file_path)
{
    pid_t revived_pid = 0;

    revived_pid = fork();
    if (CHILD == revived_pid)
    {
        if (IsRunningProcessWatchdog())
        {
            execl(*file_path, *file_path);
        }
        else
        {
            SetWDEnvVar();
            execl(WATCHDOG_PATH, *file_path);
        }
    }
    else
    {
        monitored_pid = revived_pid;
    }

    sem_wait(semaphore);

    return WD_SUCCESS;
}
