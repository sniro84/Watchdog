/************************************************ 
EXERCISE      : DS#11: Task
Implmented by : Snir Holland
Reviewed by   :  
Date          : 18/5/2023
File          : task.c
*************************************************/

#include <stddef.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include "task.h"
#include "uid.h"

enum status
{
	SUCCESS,
	MEMORY_ERR,
	FUNC_ERR
};

struct task
{
	time_t execute_time;
	size_t interval_in_sec;
	ilrd_uid_t uid;
	int (*oper_func)(void *param);
	void *operation_func_param;
	void *clean_func_param;
	void (*clean_func)(void *param);
};

task_t *TaskCreate(size_t delay,
			   	   size_t interval_in_sec,
			   	   int (*oper_func)(void *param),
				   void *operation_func_param,
				   void *clean_func_param,
				   void (*clean_func)(void *param))
{
	task_t *task = NULL;
	
	assert(NULL != oper_func);
	assert(NULL != clean_func);
	
	task = (task_t *)malloc(sizeof(task_t));
	if(NULL == task)
	{
		return NULL;
	}
	
	task->uid = UIDCreate();
	if (UIDIsSame(task->uid, UIDBadUID))
	{
		free(task);
		return NULL;
	}
	
	task->execute_time = time(NULL) + delay; 
	task->interval_in_sec = interval_in_sec;
	task->oper_func = oper_func;
	task->operation_func_param = operation_func_param;
	task->clean_func = clean_func;
	task->clean_func_param = clean_func_param;

	return task;
}

void TaskDestroy(task_t *task)
{
	assert(NULL != task);
	
	free(task);
	task = NULL;
}

int TaskRun(task_t *task)
{
	int status = 0;
	
	assert(NULL != task);
	
	status = task->oper_func(task->operation_func_param);
	
	task->clean_func(task->clean_func_param);
	
	return status;
}

int TaskIsMatch(ilrd_uid_t uid, const task_t *task)
{
	assert(NULL != task);	
	
	return (UIDIsSame(uid , task->uid));
}

void TaskUpdateTimeToRun(task_t *task)
{
	assert(NULL != task);
	
	task->execute_time += task->interval_in_sec;
	 
}

time_t TaskGetTimeToRun(const task_t *task)
{
	assert(NULL != task);
	
	return task->execute_time;
}

ilrd_uid_t TaskGetUID(const task_t *task)
{
	assert(NULL != task);
	
	return task->uid;
}

int TaskIsBefore(const task_t *task1, const task_t *task2)
{
	assert(NULL != task1);
	assert(NULL != task2);

	return (task1->execute_time < task2->execute_time); 
}

