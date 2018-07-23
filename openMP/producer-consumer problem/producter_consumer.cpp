#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#include<omp.h>
#include"producer_consumer.h"

#define OPERATION_NUM 100
#define MAX_SEND_MESSEGE 100

// #define OUTPUT

int Done(messege_queue* pool, int finished, int thread_count){
    if(  finished == thread_count/2   && pool->front == pool->back)
        return 1;
    
    return 0;
}

 int main(int argc, char const *argv[])
 {
    int thread_count;
    
    printf("please input the number of thread:");
    scanf("%d", &thread_count);

    int send_finished = 0, send_total = 0, recieve_total = 0;

    messege_queue pool;
    
    initial(&pool);

#   pragma omp parallel num_threads(thread_count) shared(pool, thread_count, send_finished, send_total, recieve_total)
    {
        int my_id = omp_get_thread_num();
        // recode my send number and my recieve number
        int my_send_times = 0;
        int my_send_total = 0;
        int my_recieve_times = 0;
        int my_recieve_total = 0;

        // act as producer
        if(my_id < thread_count/2){
            int my_messege;
            srand((unsigned int)time(NULL));
            my_messege = rand()%MAX_SEND_MESSEGE;

            for(int i = 0; i < OPERATION_NUM; i++){
                if(try_send(&pool, my_messege)){
                    my_send_times ++;
                    my_send_total += my_messege;

                    #   ifdef OUTPUT
                    printf("thread %d send messege %d succeed.\n", my_id, messege);
                    #   endif
                }

                else{
                    #   ifdef OUTPUT
                        printf("thread %d send messege fail.\n", my_id);
                    #   endif
                    };
                
                srand((unsigned int)time(NULL));
                my_messege = rand()%MAX_SEND_MESSEGE;
            }

        }

        // act as consumer
        else{
            int recieve;

            while(!Done(&pool, send_finished, thread_count)){
                // try to recieve messege
                recieve = try_recieve(&pool);
                //has messege 
                if(recieve >= 0){
                    my_recieve_times++;
                    my_recieve_total += recieve;

                    #   ifdef OUTPUT
                        printf("thread %d get messege %d succeed.\n", my_id, recieve);
                    #   endif
                }
                // don't have messege
                else{
                    #   ifdef OUTPUT
                        printf("thread %d try to get messege fail.\n", my_id);
                    #   endif
                }
            }
        }

        // producer sum up
        if(my_id < thread_count/2){
            #       pragma omp atomic
            send_finished++;

            #       pragma omp atomic
            send_total += my_send_total;

            printf("thread %d send %d.\n",my_id, my_send_total);
        }

        // consumer sum up
        else{
            #   pragma omp atomic
            recieve_total += my_recieve_total;

            printf("thread %d get %d.\n",my_id, my_recieve_total);
        }

    }//end parallel 

    destroy(&pool);

    printf("totally send:%d, totally recieve:%d\n", send_total, recieve_total);
    return 0;
 }
 