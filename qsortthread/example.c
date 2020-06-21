#include <string.h>
#include <stdio.h>
#include "qsortthread.h"

int intcmp(void * x, void * y){
  return * (int *) x - * (int *) y;
}

int mstrcmp(void * x, void * y){
  return strcmp( * (char * *) x, * (char * *) y);
}

void print_str_vec(char * v[], int size){
  for (int i = 0; i < size; i++){
    printf("[%s]", v[i]);
  }
  printf("\n");
}

void print_int_vec(int v[], int size){
  for (int i = 0; i < size; i++){
    printf("[%d]", v[i]);
  }
  printf("\n");
}


int main(void){
  char * strvec[] = {"Alan", "Tadeu", "Mike", "Jeniffer", "Oscar",
    "Margarida", "Beto", "George", "Caio"};
  int svsize = sizeof(strvec)/sizeof(char *);

  int intvec[] = {5, 3, 7, 21, 11, 2, 56, 0, 1, 34, 7, 13, 51, 44, 17, 8};
  int ivsize = sizeof(intvec)/sizeof(int);

  printf("Unsorted string vector of size %d:\n", svsize);
  print_str_vec(strvec, svsize);
  printf("Sorted vector:\n");
  quicksort(strvec, svsize, sizeof(char *), mstrcmp);
  print_str_vec(strvec, svsize);

  printf("\n\n\n");

  printf("Unsorted integer vector of size %d:\n", ivsize);
  print_int_vec(intvec, ivsize);
  printf("Sorted vector:\n");
  quicksort(intvec, ivsize, sizeof(int), intcmp);
  print_int_vec(intvec, ivsize);

}
