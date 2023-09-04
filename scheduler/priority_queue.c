/************************************************ 
EXERCISE      : DS#10: Priority Queue
Implmented by : Snir Holland
Reviewed by   : Amitai Bar 
Date          : 17/5/2023
File          : priority_queue.c
*************************************************/

#include <assert.h>
#include <stdlib.h>

#include "priority_queue.h"

enum status {SUCCESS = 0 , FAILURE = 1};

struct priority_queue 
{
	sorted_list_t *priority_queue;
};


pq_t *PQCreate(pq_compare_t cmp)
{	
	pq_t *pq = NULL;
	
	assert(NULL != cmp);
	
	pq = (pq_t *)malloc(sizeof(pq_t));
	if (NULL == pq)
	{
		return NULL;
	}
	
	pq->priority_queue = SortedListCreate(cmp);
	if (NULL == pq->priority_queue)
	{
		free(pq);
		return NULL;
	}
	
	return pq; 
}

void PQDestroy(pq_t *pq)
{
	assert(NULL != pq);
		
	SortedListDestroy(pq->priority_queue);
	free(pq);
}

int PQEnqueue(pq_t *pq, void *data)
{
	sorted_list_iter_t end = {NULL};
	sorted_list_iter_t result_iter = {NULL};

	assert(NULL != pq);
	
	result_iter = SortedListInsert(pq->priority_queue , data);
	end = SortedListEnd(pq->priority_queue);
	
	if (SortedListIsEqual(result_iter , end))
	{
		return (FAILURE);
	}
	return (SUCCESS); 
}

void *PQDequeue(pq_t *pq)
{
	void *data = NULL;

	assert(NULL != pq);
	
	if (!PQIsEmpty(pq))
	{
		data = PQPeek(pq);
		SortedListPopBack(pq->priority_queue);
	}
	
	return data;
}

void *PQPeek(const pq_t *pq)
{
	sorted_list_iter_t last_valid_iter = {NULL};

	assert(NULL != pq);
	
	last_valid_iter = SortedListPrev(SortedListEnd(pq->priority_queue));

	return SortedListGetData(last_valid_iter);
}


int PQIsEmpty(const pq_t *pq)
{
	assert(NULL != pq);
	
	return SortedListIsEmpty(pq->priority_queue);
}

size_t PQGetSize(const pq_t *pq)
{
	assert(NULL != pq);
	
	return SortedListSize(pq->priority_queue);
}


void PQClear(pq_t *pq)
{
	assert(NULL != pq);
	
	while (!SortedListIsEmpty(pq->priority_queue))
	{
		SortedListPopBack(pq->priority_queue);
	}
}

void *PQErase(pq_t *pq, pq_is_match_t func, void *param)
{
	sorted_list_iter_t where = {NULL};
	sorted_list_iter_t begin = {NULL};
	sorted_list_iter_t end = {NULL};	
		
	assert(NULL != pq);
	assert(NULL != func);
	assert(NULL != param);
	
	begin = SortedListBegin(pq->priority_queue);
	end = SortedListEnd(pq->priority_queue);
	
	where = SortedListFindIf(begin , end , func , param); 
                             
	if (!SortedListIsEqual(where , end))
	{
		param = SortedListGetData(where);	
		SortedListRemove(where);
	}
	
	return param;
}

