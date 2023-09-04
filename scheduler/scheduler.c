/************************************************ 
EXERCISE      : DS#12: Scheduler
Implmented by : Snir Holland
Reviewed by   : Adir Askayo  
Date          : 22/5/2023
File          : scheduler.c
*************************************************/

#include <stdlib.h> /* malloc */
#include <assert.h> /* assert */
#include <unistd.h> /* sleep */

#include "scheduler.h"
#include "uid.h"
#include "priority_queue.h"
#include "task.h"

struct scheduler
{
	pq_t *pq;
	int to_stop; 
	task_t *current_task;
	int to_remove_current;
};

enum boolean_status {FALSE = 0 , TRUE = 1};

/*--------------- Compare Function--------------------*/
static int TaskCmp(const void *task1, const void *task2);

/*--------------- Match Function--------------------*/
static int IsMatch(const void *task, const void *uid);

/*---------------SchedCreate--------------------*/
sched_t *SchedCreate(void)
{
	sched_t *scheduler = (sched_t *)malloc(sizeof(sched_t));
	if (NULL == scheduler)
	{
		return NULL;
	}
	
	scheduler->pq = PQCreate(&TaskCmp);
	if (NULL == scheduler->pq)
	{
		free(scheduler);
		scheduler = NULL;
		
		return NULL;
	}
	
	scheduler->to_stop = FALSE;
	scheduler->current_task = NULL;
	scheduler->to_remove_current = FALSE;
	
	return scheduler; 
	
}

/*---------------SchedDestroy--------------------*/
void SchedDestroy(sched_t *sched)
{
	assert(NULL != sched);
	
	SchedClear(sched);
	
	PQDestroy(sched->pq);
	free(sched);
	sched = NULL;
}

/*---------------SchedAdd--------------------*/
ilrd_uid_t SchedAdd(sched_t *sched,
			   size_t delay,
			   size_t interval_in_sec,
			   int (*oper_func)(void *param),
			   void *operation_func_params,
			   void *clean_func_params,
			   void (*clean_func)(void *param))
{
	task_t *task = NULL;
	
	assert(NULL != sched);
	assert(NULL != oper_func);
	assert(NULL != clean_func);
	
	task = TaskCreate(delay, interval_in_sec, oper_func, operation_func_params, clean_func_params, clean_func);
	
	if (NULL == task)
	{
		return UIDBadUID;
	}
	
	if (SUCCESS != PQEnqueue(sched->pq , task))
	{
		TaskDestroy(task);
		return UIDBadUID;
	}
	
	return TaskGetUID(task);
}

/*---------------SchedRemove--------------------*/
int SchedRemove(sched_t *sched, ilrd_uid_t uid)
{	
	task_t *task_to_remove = NULL;
	
	assert(NULL != sched);
	assert(!UIDIsSame(uid , UIDBadUID));
	
	/* a task is found inside the schedule queue */
	task_to_remove = PQErase(sched->pq , &IsMatch , &uid);
	if (NULL != task_to_remove)
	{
		TaskDestroy(task_to_remove);
		return SUCCESS;	
	}
	
	/* the current task (outside of queue) is the one to be removed */
	if (NULL != sched->current_task  &&  TaskIsMatch(uid , sched->current_task))
	{
		sched->to_remove_current = TRUE;
		return SUCCESS;
	}
	
	/* task not found */	
	return NOT_FOUND;
}

/*---------------SchedRun--------------------*/
int SchedRun(sched_t *sched)
{
	task_t *task = NULL;
	time_t timer = 0;
	int status = 0;
	
	assert(NULL != sched);
	
	while (0 == sched->to_stop  && !PQIsEmpty(sched->pq))
	{
		sched->current_task = PQDequeue(sched->pq);
		
		task = sched->current_task;
		
		timer = TaskGetTimeToRun(task);
				
		while (timer != time(NULL))
		{
			sleep(1);
		}
		
		status = TaskRun(task);
		
		switch (status)
		{
		case OP_DONE:
		case OP_ERROR:
				free(task);
				break;
		
		case OP_CONTINUE:			
				TaskUpdateTimeToRun(task);
				if (1 == sched->to_remove_current)
				{
					free(task);
					sched->to_remove_current = 0;
					break;
				}
				if (SUCCESS != PQEnqueue(sched->pq, task))
				{
					free(task);
					return MEMORY_ERR;
				}
				break;	
		}	
		sched->current_task = NULL;
	}
		
	if (1 == sched->to_stop)
	{
		sched->to_stop = 0;
		return STOPPED;
	}
		
	return NO_MORE_TASKS;
}


/*---------------SchedStop--------------------*/
void SchedStop(sched_t *sched)
{
	assert(NULL != sched);
	
	sched->to_stop = TRUE;	
}

size_t SchedSize(const sched_t *sched)
{
	assert(NULL != sched);
	
	if (NULL == sched->current_task)
	{
		return PQGetSize(sched->pq);
	}
	return PQGetSize(sched->pq) + 1;
}

void SchedClear(sched_t *sched)
{
	void *task_to_destroy = NULL;

	assert(NULL != sched);
		
	while (!PQIsEmpty(sched->pq))
	{
		task_to_destroy = PQDequeue(sched->pq); 
		TaskDestroy((task_t *)task_to_destroy);
	}
	
	if (NULL != sched->current_task)
	{
		sched->to_remove_current = TRUE;
	}
}

/*---------------SchedIsEmpty--------------------*/
int SchedIsEmpty(const sched_t *sched)
{
	assert(NULL != sched);
	
	return ((PQIsEmpty(sched->pq)) && (NULL == sched->current_task));
}


/*--------------- Compare Function--------------------*/
static int TaskCmp(const void *task1, const void *task2)
{
	assert(NULL != task1);
	assert(NULL != task2);
	
	return TaskGetTimeToRun((task_t*)task1) - TaskGetTimeToRun((task_t*)task2);
}

/*--------------- Match Function--------------------*/
static int IsMatch(const void *task, const void *uid)
{
	assert(NULL != task);
	assert(NULL != uid);
	
	return TaskIsMatch(*(ilrd_uid_t *)uid , (const task_t *)task);
}

