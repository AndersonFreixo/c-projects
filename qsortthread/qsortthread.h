#ifndef QSORTTHREAD_H
#define QSORTTHREAD_H

/*Compare function typedef*/
typedef int (* cmpfunc)(void *, void *);

/*Struct used to pack the qsort data as pthread argument*/
typedef struct part{
  void * v;
  size_t nmemb;
  size_t size;
  cmpfunc cmp;
}Partition;

/*Sorts vector 'v' with 'nmemb' members of 'size' size
using 'cmp' function to compare elements of 'v'*/
void quicksort (void * v,
              size_t nmemb,
              size_t size,
              int (*cmp)(void * arg1, void *arg2));

static void * _qsort_thread(void * part);

static int _qs_split(void * v,
              int end_i,
              size_t size,
              int (*cmp)(void * arg1, void *arg2));

static void _memswap(void * arg1, void * arg2, size_t size);

#endif
