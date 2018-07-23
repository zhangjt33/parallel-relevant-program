#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

#define MAX_MESSEGE_QUEUE_SIZE 50

// messege_queue structure
struct messege_queue{
    int *msg;
    int front;
    int back;
    omp_lock_t front_mutex;
    omp_lock_t back_mutex;
};

typedef struct messege_queue messege_queue;

// initiatl every thread and its messege_queue
void initial(messege_queue* init){
    init->msg = (int*)malloc(sizeof(int)*MAX_MESSEGE_QUEUE_SIZE);
    init->front = 0;
    init->back = 0;
    omp_init_lock(&(init->front_mutex));
    omp_init_lock(&(init->back_mutex));
}

// destroy every thread and its messege_queue 
void destroy(messege_queue* dstry){
    free(dstry->msg);
    omp_destroy_lock(&(dstry->front_mutex));
    omp_destroy_lock(&(dstry->back_mutex));
}

// try to send messege
int try_send(messege_queue *pool, int messege){
    int flag;

    omp_set_lock( &(pool->back_mutex) );

    // if the pool is full, send messege will failed.
    if( pool->front % MAX_MESSEGE_QUEUE_SIZE == pool->back % MAX_MESSEGE_QUEUE_SIZE && pool->front != pool->back) flag = 0;
    else {
        // send messege to target thread
        int t = pool->back;
        t = t % MAX_MESSEGE_QUEUE_SIZE;
        (*pool).msg[t] = messege;
        pool->back = pool->back + 1;

        // printf("thread %d get %d from send.\n", target, messege_);
        flag = 1;
    }
    omp_unset_lock( &((pool)->back_mutex));

    return flag;
};

// try to get messege
int try_recieve(messege_queue *pool){

    int messege;
   
    omp_set_lock( &(pool->front_mutex) );

    // if it is null, return -1
    if(pool->back == pool->front) messege = -1;

    else{
        // have messege to recieve
        int front = pool->front;
        front = (front)%MAX_MESSEGE_QUEUE_SIZE;
        messege = (*pool).msg[front];

        // set new front
        pool->front = pool->front + 1;

        // printf(" get %d from recieve.\n", messege);
    }
    
    omp_unset_lock( &(pool->front_mutex) );

    return messege; 
};
