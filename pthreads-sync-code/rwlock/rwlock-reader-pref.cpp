#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  rw->lock = PTHREAD_MUTEX_INITIALIZER;
  rw->read_phase = rw->write_phase = PTHREAD_COND_INITIALIZER;
  rw->resource_cnt = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  pthread_mutex_lock(&(rw->lock));
  while(rw->resource_cnt == -1){
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
  while(rw->resource_cnt != 0){
    pthread_cond_wait(&(rw->write_phase),&(rw->lock));
  }
  rw->resource_cnt = -1;
  pthread_mutex_unlock(&(rw->lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  pthread_mutex_lock(&(rw->lock));
  rw->resource_cnt = 0;
  pthread_cond_broadcast(&(rw->read_phase));
  pthread_cond_signal(&(rw->write_phase));
  pthread_mutex_unlock(&(rw->lock));
}
