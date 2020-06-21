#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "qsortthread.h"

/*Swaps two blocks of memmory*/
static void _memswap(void * arg1, void * arg2, size_t size){
  void * tmp = malloc(size);
  memcpy(tmp, arg1, size);
  memcpy(arg1, arg2, size);
  memcpy(arg2, tmp, size);
  free(tmp);
}

/*Splits vector into two partitions with numbers less or equal to pivot
and greater than pivot and returns pivot position*/
static int _qs_split(void * v,
              int end_i,
              size_t size,
              int (*cmp)(void * arg1, void *arg2))
{
  /*The pivot is always the last element of the partition.
  In each iteration, starting from the first element
  each element is compared to pivot. If it's less or equal to it
  its position is swapped with vector[sep_i] (separator index).
  sep_i starts with 0 and gets incremented by one each time there's
  a swap. In the end of the iteration, the pivot is swapped to
  the position after the last swapped element and its index is returned.*/

  void * pivot = v+(end_i * size);
  int sep_i = 0;
  void * vi; //vector [i]
  void * vsep_i; //vector [sep_i]

  for (int i = 0; i < end_i; i++){
     vi = v+(i*size);
     vsep_i = v+(sep_i * size);

     if (cmp(vi, pivot) <= 0) {
       _memswap(vsep_i, vi, size);
        ++sep_i;

     }
  }
  vsep_i = v+(sep_i * size);
  _memswap(vsep_i, v+(end_i*size), size);

  return sep_i;
}

/*Wrapper function with the same interface as
 gnulibc qsort function*/
void quicksort (void * v,
              size_t nmemb,
              size_t size,
              int (*cmp)(void * arg1, void *arg2)){

  Partition * part = malloc(sizeof(Partition));
  part -> v = v;
  part -> size = size;
  part -> nmemb = nmemb;
  part -> cmp = cmp;
//  _qsort_thread(part);
  pthread_t start;
  if (pthread_create(&start, NULL, _qsort_thread, (void *) part) == 0){
    pthread_join(start, NULL);
  }
  else{
    perror("Error making the first qsort thread!");
  }

}
/*Sorts vector 'v' with 'nmemb' members of 'size' size
using 'cmp' function to compare elements of 'v'*/
static void * _qsort_thread(void * p)
{
      Partition * part = (void *) p;
      void * v =      part -> v;
      size_t size =   part -> size;
      size_t nmemb =  part -> nmemb;
      cmpfunc cmp =   part -> cmp;

      int end_i = nmemb - 1;
      int pivot_i = _qs_split(v, end_i, size, cmp);

      /*Members of left partition start from v[0] to [pivot_i -1]*/
      /*Members of right partition start from
      v[pivot_i+1] to v[nmemb-1]*/

      int l_nmemb = pivot_i;
      int r_nmemb = nmemb-(pivot_i+1);

      /*left vector to be rearranged starts in the same address as
      current v, and the right vector starts after pivot*/
      void * r_v = v +(pivot_i+1)*size;

      /*No need to split vector partition with a single member*/

      pthread_t left_th = 0, right_th = 0;
      if (l_nmemb > 1){
        Partition *left = malloc(sizeof(Partition));
        left -> v     = v;
        left -> nmemb = l_nmemb;
        left -> size  = size;
        left -> cmp   = cmp;
        pthread_create(&left_th, NULL, _qsort_thread, left);
      }

      if (r_nmemb > 1){
      //  printf("\nr_nmemb=%d", r_nmemb);
        Partition *right = malloc(sizeof(Partition));
        right -> v      = r_v;
        right -> nmemb  = r_nmemb;
        right -> size   = size;
        right -> cmp  = cmp;
        pthread_create(&right_th, NULL, _qsort_thread, right);

      }
      if (left_th) pthread_join(left_th, NULL);
      if (right_th) pthread_join(right_th, NULL);

      free(p);
}
