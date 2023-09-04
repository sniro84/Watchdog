/************************************************ 
EXERCISE      : DS#7: Doubly Linked List
Implmented by : Snir Holland
Reviewed by   : Noga Rotem
Date          : 6/5/2023
File          : dlist.c
*************************************************/


#include <stddef.h>  /* size_t */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* malloc */

#include "dlist.h" 

enum status {SUCCESS = 0 , FAILURE = 1};

#define FOUND    (1)

typedef struct dlist_node
{
    void *data;
    struct dlist_node *next;
    struct dlist_node *prev;
} dlist_node_t;

struct dlist
{
    dlist_node_t head;
    dlist_node_t tail;
};

/* Helper functions */
static int Increment(void *nothing , void *counter);

static void BounceToEnd(dlist_iter_t iter);

/*-----------------DListCreate-----------------*/
dlist_t *DListCreate(void)
{
	dlist_t *list = (dlist_t *)malloc(sizeof(dlist_t));
	if (NULL == list)
	{
		return NULL;
	}
	
	(list -> head).data = NULL;
	(list -> head).next = &(list -> tail);
	(list -> head).prev = NULL;
	
	(list -> tail).data = NULL;
	(list -> tail).next = NULL;
	(list -> tail).prev = &(list -> head);
	
	return (list);
}	
	
/*-----------------DListDestroy-----------------*/
void DListDestroy(dlist_t *list)
{
	dlist_iter_t runner = NULL;
	dlist_iter_t temp = NULL;
	
	assert(NULL != list);
	
	runner = list -> head.next;
	
	while (NULL != runner -> next)
	{
		temp = runner;
		runner = runner -> next;
		free(temp);
	}
	
	free(list);
}

/*-----------------DListIsEqual-----------------*/
int DListIsEqual(dlist_iter_t left_iter, dlist_iter_t right_iter)
{
	assert(NULL != left_iter);
	assert(NULL != right_iter);
	
	return (left_iter == right_iter);
}

/* -----------------DListGetBegin-----------------*/
dlist_iter_t DListGetBegin(const dlist_t *list)
{
	assert(NULL != list);
	
	return (list -> head).next;
}

/* -----------------DListGetEnd-----------------*/
dlist_iter_t DListGetEnd(const dlist_t *list)
{
	assert(NULL != list);
	
	/* IMPORTANT!: returns an invalid iterator */
	return (dlist_iter_t)&(list -> tail);  
}

/*-----------------DListIsEmpty-----------------*/
int DListIsEmpty(dlist_t *list)
{
	assert(NULL != list);
	return (list -> head).next == (dlist_iter_t)&(list -> tail);
}

/*-----------------DListNext-----------------*/
dlist_iter_t DListNext(dlist_iter_t iter)
{
	assert(NULL != iter);
	
	return (iter -> next);
}

/*-----------------DListPrev-----------------*/
dlist_iter_t DListPrev(dlist_iter_t iter)
{
	assert(NULL != iter);
	
	return (iter -> prev);
}

/*---------------DListGetData---------------*/
void *DListGetData(dlist_iter_t iter)
{
	assert(NULL != iter);

	return (iter -> data);
}

/*-----------------DListInsert---------------*/
dlist_iter_t DListInsert(dlist_iter_t where, void *data)
{
	dlist_node_t *new_node = NULL;
	
	assert(NULL != where);
	
	/* memory allocation for the new node */
	new_node = (dlist_node_t*)malloc(sizeof(dlist_node_t));
	if (NULL == new_node)
	{
		BounceToEnd(where); 
		return (where);
	}
	
	/* updating values of new node */	
	new_node -> data = data;
	new_node -> next = where;
	new_node -> prev = where -> prev;
	
	/* updating iterator where */
	where -> prev -> next = new_node;	
	where -> prev = new_node;
	
	return (new_node);
}

/*-----------------DListRemove---------------*/
dlist_iter_t DListRemove(dlist_iter_t iter)
{
	dlist_iter_t updated_iter = NULL;
	
	/* iterator must be valid */
	assert(NULL != iter);
	assert(NULL != iter -> next);
	assert(NULL != iter -> prev);

	/* adjusting the pointers of iter's neighboors */
	iter -> prev -> next = iter -> next;
	iter -> next -> prev = iter -> prev;
	
	/* store iterator to be returned */
	updated_iter = iter -> next;
	
	/* free deleted iterator */
	free(iter);
	
	return (updated_iter);
}

/*-----------------DListCount---------------*/
size_t DListCount(const dlist_t *list)
{
	size_t count = 0;
	dlist_iter_t begin = NULL;	
	dlist_iter_t end = NULL;

	assert(NULL != list);
	
	begin = DListGetBegin(list);
	end = DListGetEnd(list);
	
	DListForEach(begin , end , &Increment, &count);
	
	return (count); 
}

/*-----------------DListForEach---------------*/
int DListForEach(dlist_iter_t from, dlist_iter_t to, action_func_t action_func, void *param)
{
	int status = SUCCESS;	
	
	dlist_iter_t runner = NULL;
	
	assert(NULL != from);
	assert(NULL != to);
	assert(NULL != action_func);
	
	runner = from;
	
	while (runner != to)
	{
		status = (*action_func)(runner -> data , param);
		if (FAILURE == status)
		{
			break;
		}
		runner = runner -> next;		
	}
	
	return (status);
}

/*-----------------DListPushFront---------------*/
dlist_iter_t DListPushFront(dlist_t *dlist, void *data)
{
	dlist_iter_t first = NULL;	

	assert(NULL != dlist);	
	
	first = DListGetBegin(dlist);
	
	return DListInsert(first, data);
}

/*-----------------DListPushEnd---------------*/
dlist_iter_t DListPushEnd(dlist_t *dlist, void *data)
{
	dlist_iter_t last = NULL;	
	
	assert(NULL != dlist);	
	
	last = DListGetEnd(dlist);
	
	return DListInsert(last, data);
}

/*-----------------DListPopFront---------------*/
void *DListPopFront(dlist_t *dlist)
{
	void *data = NULL;	

	assert(NULL != dlist);	
	
	data = DListGetData(DListGetBegin(dlist));

	DListRemove(DListGetBegin(dlist));
	
	return (data);
}

/*-----------------DListPopEnd---------------*/
void *DListPopEnd(dlist_t *dlist)
{
	dlist_iter_t last_valid_iter = NULL;
	void *data = NULL;	

	assert(NULL != dlist);	
		
	last_valid_iter = DListGetEnd(dlist) -> prev;
	
	data = DListGetData(last_valid_iter);

	DListRemove(last_valid_iter);
	
	return (data);
}

dlist_iter_t DListFind(dlist_iter_t from, dlist_iter_t to, match_func_t match_func, const void *param)
{
	dlist_iter_t runner = NULL;
	
	assert(NULL != from);
	assert(NULL != to);
	assert(NULL != match_func);
	
	runner = from;
	
	while (runner != to)
	{
		if ((FOUND == (*match_func)(runner -> data , param)))
		{
			break;
		}
		runner = runner -> next;		
	}
	
	return (runner);
}

dlist_iter_t DListSplice(dlist_iter_t from, dlist_iter_t to, dlist_iter_t where)
{
	/* temporary iterators */
	dlist_iter_t old_from_prev = NULL;
	dlist_iter_t old_to_prev = NULL;
	dlist_iter_t old_where_prev = NULL;
	
	assert(from);
	assert(to);
	assert(where);
	

	/* updating nexts */
	where -> prev -> next = from;
	from -> prev -> next = to;
	to -> prev -> next = where;
	
	
	/* storing old values of prevs */
	old_from_prev = from -> prev;
	old_to_prev = to -> prev;
	old_where_prev = where -> prev;
	
	/* updating prevs */
	from -> prev = old_where_prev;
	where -> prev = old_to_prev;
	to -> prev = old_from_prev;
	
	return (where); 
}

int DListMultiFind(dlist_iter_t from, dlist_iter_t to, match_func_t match_func , const void *param, dlist_t *output_list)
{
	dlist_iter_t runner = NULL;

	assert(NULL != from);
	assert(NULL != to);
	assert(NULL != match_func);
	
	runner = from;
	
	while (runner != to)
	{	  
		if (FOUND == (*match_func)(runner -> data , param))
		{
			if (DListIsEqual(DListPushEnd(output_list, runner -> data) , DListGetEnd(output_list)))
			{
				return FAILURE;
			}
		}
		runner = runner -> next;		
	}
	
	return (DListIsEmpty(output_list)) ? (FAILURE) : (SUCCESS); 
}

/* Helper function to increment node count */
static int Increment(void *nothing , void *counter)
{
	++(*(size_t*)counter);
	(void)nothing;
	
	return (SUCCESS);	
}

/* Helper function that bounces to invalid iterator */
static void BounceToEnd(dlist_iter_t iter)
{
	assert(NULL != iter);	
	
	while (NULL != iter -> next)  
	{
		iter = iter -> next;
	}
}
