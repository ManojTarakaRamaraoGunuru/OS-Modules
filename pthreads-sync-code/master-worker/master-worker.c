#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size, item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t is_empty = PTHREAD_COND_INITIALIZER, is_full = PTHREAD_COND_INITIALIZER;

int add ;/*position to add in the buffer*/
int rem; /*position from where number is consumed*/


int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(item_to_produce < total_items){
    pthread_mutex_lock(&lock);
    while(curr_buf_size == max_buf_size && item_to_produce<total_items){
      pthread_cond_wait(&is_full, &lock);
    }
    buffer[add] = item_to_produce;
    print_produced(buffer[add], thread_id);
    add = (add + 1)%max_buf_size;
    item_to_produce++;
    curr_buf_size++;
    pthread_mutex_unlock(&lock);

    pthread_cond_signal(&is_empty);
  }

  return 0;
}

void *process_request_loop(void*args){
  int thread_id = *(int*)args;

  while(item_to_consume<total_items){
    pthread_mutex_lock(&lock);
    while(curr_buf_size == 0 && item_to_consume<total_items){
      pthread_cond_wait(&is_empty, &lock);
    }
    print_consumed(buffer[rem], thread_id);
    rem = (rem+1)%max_buf_size;
    curr_buf_size--;
    item_to_consume++;
    pthread_mutex_unlock(&lock);

    pthread_cond_signal(&is_full);
  }

}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
  int *master_thread_id, *worker_thread_id;
  pthread_t *master_thread, *worker_thread;
  item_to_produce = 0;
  curr_buf_size = 0;
  item_to_consume = 0;
  add =0, rem = 0;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

   worker_thread = (pthread_t*)malloc(sizeof(pthread_t)*num_workers);
   worker_thread_id = (int*)malloc(sizeof(int)*num_workers);
   for(int i = 0; i<num_workers;i++){
    worker_thread_id[i] = i;
   }

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  for(int i = 0; i<num_workers;i++){
    pthread_create(&worker_thread[i], NULL,process_request_loop, (void*)&worker_thread_id[i]);
  }

  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++){
      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
  }

  for(int i = 0; i<num_workers;i++){
      pthread_join(worker_thread[i],NULL);
      printf("worker %d joined\n", i);
  }

  
  /*----Deallocating Buffers---------------------*/
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&is_empty);
  pthread_cond_destroy(&is_full);

  free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread);
  free(worker_thread_id);
  
  return 0;
}
