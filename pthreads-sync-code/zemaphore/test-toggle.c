#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define NUM_THREADS 3
#define NUM_ITER 10

// Goal is to print 0,1,2,0,1,2,0,1,2 and so on using zeamaphores

// This exercise is mainly to understand the fact seamphores can allow threads to (group or multiple)access the common resource at a time
// thread which does sema_down need not up the semaphore 

zem_t zero, first, second;

void *justprint(void *data){
  int thread_id = *((int *)data);

  for(int i=0; i < NUM_ITER; i++){
      
    if(thread_id == 0){
      zem_down(&zero);
    }else if(thread_id == 1){
      zem_down(&first);
    }else if(thread_id == 2){
      zem_down(&second);
    }
    printf("This is thread %d\n", thread_id);

    if(thread_id == 0){
      zem_up(&first);
    }else if(thread_id == 1){
      zem_up(&second);
    }else if(thread_id == 2){
      zem_up(&zero);
    }

  }
  
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  zem_init(&zero, 1);
  zem_init(&first, 0); 
  zem_init(&second, 0);
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
