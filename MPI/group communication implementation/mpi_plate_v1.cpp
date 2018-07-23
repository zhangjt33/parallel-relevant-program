#include<stdio.h>
#include<string.h>
#include<mpi.h>

// compile:
// mpic++ -g -Wall -o mpi_plate_v1 mpi_plate_v1.cpp

// run:
// mpirun -np <number of process> ./mpi_plate_v1

int main(void){

    int     comm_sz;    //number of process
    int     my_rank;    //my process rank

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


    int current_step_size = 2;
    int my_sum = my_rank + 1, data_receive, dst;

    // loop until all process incolve in one set
    while(current_step_size <= comm_sz){

        // it is left part of small set, partner is in right
        if(my_rank% current_step_size < current_step_size/2){
            dst = my_rank + current_step_size/2;
        }
        // else it is right part of set,partner is in left
        else{
            dst = my_rank - current_step_size/2;
        }

        // exchange data with partner
        MPI_Sendrecv(&my_sum, 1, MPI_INT, dst, 0,
                    &data_receive, 1, MPI_INT, dst, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        my_sum += data_receive;

        // updata current_step_size
        current_step_size *= 2;
    }

    printf("process %d of %d > get total sum is: %d.\n", my_rank, comm_sz, my_sum); 

    MPI_Finalize();

    return 0;
}