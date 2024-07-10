#include "rwlock.h"

int waiting_writers = 0;

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  pthread_mutex_init(&(rw->lock), NULL);
  pthread_cond_init(&(rw->read_phase), NULL);
  pthread_cond_init(&(rw->write_phase), NULL);
  rw->resource_cnt = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&(rw->lock));
  while(rw->resource_cnt == -1 || (waiting_writers>0)){
    pthread_cond_wait(&(rw->read_phase),&(rw->lock));
  }
  rw->resource_cnt++;
  pthread_mutex_unlock(&(rw->lock));
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&(rw->lock));
  rw->resource_cnt--;
  if(rw->resource_cnt == 0){
    pthread_cond_signal(&(rw->write_phase));
  }
  pthread_mutex_unlock(&(rw->lock));
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  pthread_mutex_lock(&(rw->lock));
  waiting_writers++;
  while(rw->resource_cnt != 0){
    pthread_cond_wait(&(rw->write_phase),&(rw->lock));
  }
  waiting_writers--;
  rw->resource_cnt = -1;
  pthread_mutex_unlock(&(rw->lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&(rw->lock));
  rw->resource_cnt = 0;
  if(waiting_writers>0){
    pthread_cond_signal(&(rw->write_phase));
  }else{
    pthread_cond_broadcast(&(rw->read_phase));
  }
  pthread_mutex_unlock(&(rw->lock));
}
