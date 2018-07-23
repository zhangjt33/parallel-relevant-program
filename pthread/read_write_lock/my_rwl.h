#include <pthread.h>

// #define WRITE_FIRST 

// self-definition read-write-lock
struct my_rwlock_t{

    // use to lock itself
    pthread_mutex_t mutex;

    // read conditional lock
    pthread_cond_t  read;

    //  write conditional lock
    pthread_cond_t  write;

    // record read and write threads
    int read_now;
    int read_wait;
    int write_now;
    int write_wait;
};

typedef struct my_rwlock_t my_rwlock_t;

// initial read-write-lock
void my_rwlock_init(my_rwlock_t* rwlock){
    pthread_mutex_init(&(rwlock->mutex), NULL);

    pthread_cond_init(&(rwlock->read), NULL);

    pthread_cond_init(&(rwlock->write), NULL);

    rwlock->read_now = 0;
    rwlock->read_wait = 0;
    rwlock->write_now = 0;
    rwlock->write_wait = 0;
};

// destroy read-write-lock
void my_rwlock_destroy(my_rwlock_t* rwlock){
    pthread_mutex_destroy(&(rwlock->mutex));

    pthread_cond_destroy(&(rwlock->read));

    pthread_cond_destroy(&(rwlock->write));
};

// read lock
void my_rwlock_rdlock(my_rwlock_t* rwlock){

    pthread_mutex_lock(&(rwlock->mutex));

#   ifdef WRITE_FIRST
    // write first
    // if it is writing or some writer is waiting
    if(rwlock->write_wait > 0 || rwlock->write_now > 0) {
        rwlock->read_wait = rwlock->read_wait + 1;
        // wait here
        pthread_cond_wait(&(rwlock->read), &(rwlock->mutex));

        // wake up
        rwlock->read_wait = rwlock->read_wait - 1;
        rwlock->read_now = rwlock->read_now + 1;
    }
    else rwlock->read_now = rwlock->read_now + 1;
#   else
    // read first
    // if no write now
    if(rwlock->write_now == 0) rwlock->read_now = rwlock->read_now + 1;
    // writing now, this thread have to wait 
    else{
        rwlock->read_wait = rwlock->read_wait + 1;

        // wait here
        pthread_cond_wait(&(rwlock->read), &(rwlock->mutex));

        // wake up
        rwlock->read_wait = rwlock->read_wait - 1;
        rwlock->read_now = rwlock->read_now + 1;
    }
#   endif
    pthread_mutex_unlock(&(rwlock->mutex));
};

// write lock
void my_rwlock_wrlock(my_rwlock_t* rwlock){
    pthread_mutex_lock(&(rwlock->mutex));

    // no read and no write
    if(rwlock->read_now == 0 && rwlock->write_now == 0) rwlock->write_now = rwlock->write_now + 1;

    // reading or writing
    else{
        rwlock->write_wait = rwlock->write_wait + 1;

        // wait here
        pthread_cond_wait(&(rwlock->write), &(rwlock->mutex));

        // wake up
        rwlock->write_wait = rwlock->write_wait - 1;
        rwlock->write_now = rwlock->write_now + 1;
    }
    pthread_mutex_unlock(&(rwlock->mutex));
};

void my_rwlock_unlock(my_rwlock_t* rwlock){
    pthread_mutex_lock(&(rwlock->mutex));

    // if it is a read thread and there are many readers.
    if(rwlock->read_now > 1) rwlock->read_now = rwlock->read_now - 1;

    // if it is a read thread and there is only one reader.
    else if (rwlock->read_now == 1){
        rwlock->read_now = rwlock->read_now - 1;

        // wake up a writer if needed
        if(rwlock->write_wait > 0) pthread_cond_signal(&(rwlock->write));
    }

    // if it is a writer
    else{
        rwlock->write_now = rwlock->write_now - 1;

        # ifndef WRITE_FIRST
        // read first
        if(rwlock->read_wait > 0) pthread_cond_broadcast(&(rwlock->read));

        else if(rwlock->write_wait > 0) pthread_cond_signal(&rwlock->write);
        #else
        // write first 
        if(rwlock->write_wait > 0) pthread_cond_signal(&rwlock->write);
        else if(rwlock->read_wait > 0) pthread_cond_broadcast(&(rwlock->read));

        # endif
    }

    pthread_mutex_unlock(&(rwlock->mutex));
};