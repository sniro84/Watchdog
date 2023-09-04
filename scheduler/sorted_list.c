/************************************************ 
EXERCISE      : DS#8: Sorted Doubly Linked List
Implmented by : Snir Holland
Reviewed by   : Adir Askayo 
Date          : 7/5/2023
File          : sorted_list.c
*************************************************/


#include <stddef.h>  /* size_t */
#include <assert.h>  /* assert */
#include <stdlib.h>  /* malloc */

#include "sorted_list.h"

static sorted_list_iter_t DIterToSortIter(dlist_iter_t d_iter ,sorted_list_t *list);
static dlist_iter_t SortIterToDIter(sorted_list_iter_t iter);
static int LocationToInsert(const void *iter_data , const void *util);
static int BooleanMatch(const void *iter_data , const void *util); 

struct sorted_list
{
	dlist_t *dlist;
	sorted_list_cmp_t cmp;
};

typedef struct cmp_util
{
	sorted_list_cmp_t cmp;
	const void *data;
} cmp_util_t;

/*-----------------SortedListCreate-----------------*/
sorted_list_t *SortedListCreate(sorted_list_cmp_t func)
{
	sorted_list_t *list = NULL;
	
	assert(NULL != func);
	
	list = (sorted_list_t *)malloc(sizeof(sorted_list_t) );
	if (NULL == list)
	{
		return NULL;
	}
	
	list->dlist = DListCreate();
	if (NULL == list->dlist)
	{
		free(list);
		return NULL;
	}
	
	list->cmp = func;
	
	return list;
}
		
/*-----------------SortedListDestroy-----------------*/
void SortedListDestroy(sorted_list_t *list)
{
	assert(NULL != list);
	
	DListDestroy(list->dlist);
	free(list);
}

/*-----------------SortedListInsert-----------------*/
sorted_list_iter_t SortedListInsert(sorted_list_t *list, void *data_to_insert)
{
	/* first and last dlist iterators */
	dlist_iter_t begin = NULL;
	dlist_iter_t end = NULL;
	
	/* a struct to help us perform the insertion */
	cmp_util_t util = {NULL};

	/* we want to insert the data before this where iterator */
	dlist_iter_t where = NULL;
	
	/* the iterator we want to insert */
	dlist_iter_t new_iter = NULL;
	
	assert(NULL != list); 
	
	begin = DListGetBegin(list->dlist);
	end = DListGetEnd(list->dlist);
	
	util.cmp = list->cmp;
	util.data = data_to_insert; 
		
	where = DListFind(begin , end , &LocationToInsert , &util);
	
	new_iter = DListInsert(where , data_to_insert);
	
	return DIterToSortIter(new_iter , list);
}

/*-----------------SortedListFind-----------------*/
sorted_list_iter_t SortedListFind(sorted_list_t *list, sorted_list_iter_t from, sorted_list_iter_t to, const void *to_find)
{
	dlist_iter_t d_from = NULL;
	dlist_iter_t d_to = NULL;
	dlist_iter_t d_result = NULL;
	
	cmp_util_t util = {NULL};	
	
	assert(NULL != list);
	assert(from.list == to.list);
	
	util.cmp = list->cmp;
	util.data = (void*)to_find; 

	d_from = SortIterToDIter(from);
	d_to = SortIterToDIter(to);
	
	d_result = DListFind(d_from , d_to , &BooleanMatch , &util);	
	
	return DIterToSortIter(d_result , list);
}

/*-----------------SortedListRemove-----------------*/
sorted_list_iter_t SortedListRemove(sorted_list_iter_t iter)
{
	dlist_iter_t where = SortIterToDIter(iter);
	
	iter.internal_iter = DListRemove(where);
	
	return iter;
}

/*-----------------SortedListGetData-----------------*/
void *SortedListGetData(sorted_list_iter_t cur)
{
	dlist_iter_t d_curr = SortIterToDIter(cur);
	
	return DListGetData(d_curr);
}

/*-----------------SortedListPopFront-------------------------- */
void *SortedListPopFront(sorted_list_t *list)
{
	assert(NULL != list);
	
	return DListPopFront(list->dlist);
}

/*-----------------SortedListPopBack-------------------------- */
void *SortedListPopBack(sorted_list_t *list)
{
	assert(NULL != list);
	
	return DListPopEnd(list->dlist);
}

/*-----------------SortedListSize-------------------------- */
size_t SortedListSize(const sorted_list_t *list)
{
	assert(NULL != list);
	return DListCount(list -> dlist);
}

/*-----------------SortedListForEach-------------------------- */
int SortedListForEach(sorted_list_iter_t from, sorted_list_iter_t to, action_func_t func, void *func_param)
{
	dlist_iter_t d_from = NULL;
	dlist_iter_t d_to = NULL;	
	
	assert(NULL != func);
	assert(from.list == to.list);
	
	d_from = SortIterToDIter(from);
	d_to = SortIterToDIter(to);
	
	return DListForEach(d_from , d_to , func , func_param);
}

/*-----------------SortedListBegin-------------------------- */
sorted_list_iter_t SortedListBegin(const sorted_list_t *list)
{
	assert(NULL != list);
	
	return DIterToSortIter(DListGetBegin(list -> dlist) , (sorted_list_t *)list);
}

/*-----------------SortedListEnd-------------------------- */
sorted_list_iter_t SortedListEnd(const sorted_list_t *list)
{
	assert(NULL != list);
	
	return DIterToSortIter(DListGetEnd(list -> dlist) , (sorted_list_t *)list);
}

/*-----------------SortedListNext-------------------------- */
sorted_list_iter_t SortedListNext(sorted_list_iter_t cur)
{
	dlist_iter_t d_curr = SortIterToDIter(cur);
	
	#ifndef NDEBUG
	return DIterToSortIter(DListNext(d_curr), cur.list);	
	#else
	return DIterToSortIter(DListNext(d_curr), NULL);
	#endif
	
}

/*-----------------SortedListPrev-------------------------- */
sorted_list_iter_t SortedListPrev(sorted_list_iter_t cur)
{
	dlist_iter_t d_curr = SortIterToDIter(cur);

	#ifndef NDEBUG
	return DIterToSortIter(DListPrev(d_curr), cur.list);	
	#else
	return DIterToSortIter(DListPrev(d_curr), NULL);
	#endif
}

/*-----------------SortedListIsEmpty------------------------ */
int SortedListIsEmpty(const sorted_list_t *list)
{
	assert(NULL != list);
	
	return DListIsEmpty(list->dlist);
}

/*-----------------SortedListMerge-------------------------- */
void SortedListMerge(sorted_list_t *dest, sorted_list_t *src)
{
	dlist_iter_t from = NULL;
	dlist_iter_t to = NULL;
	dlist_iter_t where = NULL;
	
	dlist_iter_t end_src = NULL;
	dlist_iter_t end_dest = NULL;
	
	cmp_util_t util = {NULL};
	
	assert(NULL != dest);
	assert(NULL != src);

	util.cmp = dest->cmp;
		
	where = DListGetBegin(dest->dlist);
	
	end_src = DListGetEnd(src->dlist);
	end_dest = DListGetEnd(dest->dlist);
	
	while (!SortedListIsEmpty(src))
	{
		/* always start from the beginning of source list */ 
		from = DListGetBegin(src->dlist);
		
		/* figure out the starting position of the splice */		
		util.data = DListGetData(from); 
		where = DListFind(where , end_dest , &LocationToInsert , &util);
		
		/* an edge case: where is the end of dest */
		if (DListIsEqual(end_dest , where))
		{
			to = end_src;
			DListSplice(from , to , where);
			break; 
		}
		
		/* figure out the ending position of the splice */
		util.data = DListGetData(where); 
		to = DListFind(from , end_src , &LocationToInsert , &util);
		
		/* perform the splice */
		DListSplice(from , to , where);   
	}
}

/*-----------------SortedListIsEqual-------------------------- */
int SortedListIsEqual(sorted_list_iter_t it1 , sorted_list_iter_t it2)
{
	dlist_iter_t d_it1 = SortIterToDIter(it1);
	dlist_iter_t d_it2 = SortIterToDIter(it2);
	
	return DListIsEqual(d_it1 , d_it2);
}

/*-----------------SortedListFindIf-----------------*/
sorted_list_iter_t SortedListFindIf(sorted_list_iter_t from, sorted_list_iter_t to, match_func_t func, const void *param)
{
	dlist_iter_t d_from = NULL;
	dlist_iter_t d_to = NULL;
	
	assert(NULL != func);
	assert(from.list == to.list);

	d_from = SortIterToDIter(from);
	d_to = SortIterToDIter(to);	
	
	from.internal_iter = DListFind(d_from , d_to , func , param);	
	
	return from;
}

/* ------------Helper Functions------------ */

/* This function converts a SortedList iter to DList iter */
static dlist_iter_t SortIterToDIter(sorted_list_iter_t iter)
{
	return (iter.internal_iter);
}

/* This function converts DList iter to SortedList iter */
static sorted_list_iter_t DIterToSortIter(dlist_iter_t d_iter ,sorted_list_t *list)
{
	sorted_list_iter_t result = {NULL};
	
	result.internal_iter = d_iter;	
	
	#ifndef NDEBUG
	result.list = list;
	#else
	(void)list;
	#endif
	
	return (result);	
}

static int LocationToInsert(const void *iter_data , const void *util)
{
	sorted_list_cmp_t cmp = ((cmp_util_t*)util)->cmp;
	const void *data_to_insert  = ((cmp_util_t*)util)->data;
	
	return cmp(iter_data , data_to_insert) >= 0;	
}

static int BooleanMatch(const void *iter_data , const void *util)
{
	sorted_list_cmp_t cmp = ((cmp_util_t*)util)->cmp;
	const void *data_to_insert  = ((cmp_util_t*)util)->data;
	
	return cmp(iter_data , data_to_insert) == 0;	
}

