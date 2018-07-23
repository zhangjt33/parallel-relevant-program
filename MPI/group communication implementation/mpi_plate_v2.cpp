#include<stdio.h>
#include<string.h>
#include<mpi.h>

// compile:
// mpic++ -g -Wall -o mpi_plate_v1 mpi_plate_v2.cpp

// run:
// mpirun -np <number of process> ./mpi_plate_v2

// find a less int that is large than init_value and is power of 2
int adaptor(int init_value){
    int result = 2;
    if(init_value <= 2) return init_value;

    while(result < init_value) result*=2;

    // printf("adaptor is :%d\n",result);
    return result; 
}

int main(void){
    int     comm_sz;    //number of process
    int     my_rank;    //my process rank

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int adapt_length = adaptor(comm_sz);
    int current_comm_size = 2;
    int my_sum = my_rank + 1, receive_data, dst;

    // loop until all process incolve in one set
    while(current_comm_size <= adapt_length){

        // calculate parter
        // it is left part of small set, partner is in right
        if(my_rank% current_comm_size < current_comm_size/2){
            dst = my_rank + current_comm_size/2;
        }
        // else it is right part of set,partner is in left
        else{
            dst = my_rank - current_comm_size/2;
        }

        // exchange data with partner, but the parter should be invalid
        if(dst <= comm_sz - 1){
            MPI_Sendrecv(&my_sum, 1, MPI_INT, dst, 0,
                    &receive_data, 1, MPI_INT, dst, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
            my_sum += receive_data;
        }

        // else it should be corrected, with current_comm_size is larger than 2
        else if(current_comm_size > 2){
            // from leftest partner to get sum
            dst = my_rank - my_rank%current_comm_size;
            if(dst != my_rank) 
            MPI_Recv(&my_sum, 1, MPI_INT, dst, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        //the leftest process of left part of rightest set should act as corrector, and it only correct when necessary
        if(my_rank%current_comm_size == 0 && my_rank + current_comm_size > comm_sz){

            //there are (my_rank + current_comm_size - comm_sz) processes to correct
            int middle = my_rank + current_comm_size/2;
            for(int no_partner_count = my_rank + current_comm_size - comm_sz; no_partner_count > 0 ; no_partner_count--){
                dst = middle - no_partner_count;
                if(dst > my_rank && dst < comm_sz)
                MPI_Send(&my_sum, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
            }              
        }
        
        // updata current_comm_size
        current_comm_size *= 2;
    }

    printf("process %d of %d > get total sum is: %d.\n", my_rank, comm_sz, my_sum); 

    MPI_Finalize();

    return 0;
}