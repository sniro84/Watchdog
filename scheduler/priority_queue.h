/************************************************ 
EXERCISE      : DS#10: Priority Queue
Implmented by : Snir Holland
Reviewed by   : Amitai Bar 
Date          : 17/5/2023
File          : priority_queue.h
*************************************************/

#ifndef __PRIORIT_QUEUE_H__
#define __PRIORIT_QUEUE_H__

#include <stddef.h>
#include "sorted_list.h" 

/*
*    Compare function between data and parm.
*
*    Arguments:
*        data - the data to do compare.
*        param - the param to do compare.
*
*    Return: 0 if isnt math 1 if match
*
*/
typedef int (*pq_is_match_t)(const void *data, const void *param);

typedef int (*pq_compare_t)(const void *data_1, const void *data_2);

typedef struct priority_queue pq_t;

/*
*
*	creates a new Queue
*
*	Arguments:
*		cmp - compare function.
*
*	Return: a new priority Queue if successful
*				otherwise return NULL.
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
pq_t *PQCreate(pq_compare_t cmp);

/*
*
*	frees all memory allocated for the Queue
*
*	Arguments:
*		pq - priority Queue to destroy. must be a valid address.
*
*	Return: void.
*
*	Time complexity: O(n) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
void PQDestroy(pq_t *pq);

/*
*
*	add a value to the end of the Queue
*
*	Arguments:
*		pq - pointer to the queue to change. must be vaild.
*		data - The value that we wish to add to the end of the Queue.
*
*
*	Return: 0 - success,
*			1- failure.
*
*	Time complexity: O(n) best/average, O(n) - worst
*	Space complexity: O(1) best/average/worst
*
*/
int PQEnqueue(pq_t *pq, void *data);

/*
*
*	remove a value from the front of the Queue
*
*	Arguments:
*		pq - pointer to the queue to change. must be vaild.
*
*	Return: data of dequeued element.
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
void *PQDequeue(pq_t *pq);

/*
*
*	return the value at the front of the Queue.
*
*	Arguments:
*		pq - pointer to the queue to check. must be vaild.
*
*	Return: A pointer to the value at the front of the Queue.
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
void *PQPeek(const pq_t *pq);

/*
*
*	Checks if the Queue is empty
*
*	Arguments:
*		pq - pointer to the queue to check. must be vaild.
*
*	Return: 1 - if the queue is empty, 0 - otherwise
*
*	Time complexity: O(1) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
int PQIsEmpty(const pq_t *pq);

/*
*
*	Return the number of current values in the Queue 
*
*	Arguments:
*		pq - pointer to the queue to check. must be vaild.
*
*	Return: number of values in the queue 
*
*	Time complexity: O(n) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
size_t PQGetSize(const pq_t *pq);


/*
*
*	Remove all the elements from the Queue 
*
*	Arguments:
*		pq - pointer to the queue to be cleared. must be vaild.
*
*	Return: void. 
*
*	Time complexity: O(n) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
void PQClear(pq_t *pq);

/*
*
*	Erase the first occurrence of an element in the queue 
*
*	Arguments:
*		pq - pointer to the queue to search for the data. must be vaild.
*		func - boolean match function
		param - the param to match
*	Return: Erased data. 
*
*	Time complexity: O(n) best/average/worst
*	Space complexity: O(1) best/average/worst
*
*/
void *PQErase(pq_t *pq, pq_is_match_t func, void *param);


#endif /* __PRIORIT_QUEUE_H__ */
