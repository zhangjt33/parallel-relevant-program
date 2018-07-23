#include<stdio.h>
#include<string.h>
#include<mpi.h>


// compile:
// mpic++ -g -Wall -o mpi_plate_v2 mpi_tree_v2.cpp

// run:
// mpirun -np <number of process> ./mpi_tree_v2

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

    int adapt_length = adaptor(comm_sz);// get a less number that is power of 2 to adapt algorithm

    int current_comm_size = adapt_length ; 
    int local_sum = my_rank + 1, receive_data = 0, total_sum = 0, dst;

// first, calculate total sum in 0 process: 
    // only in this case should process operate send or receive, or it has already finish its work!
    while(current_comm_size > 1 && my_rank < current_comm_size){

        // in this case, process should send and exit
        if( my_rank >= current_comm_size/2 ){
            
            // destination is my_rank - current_comm_size/2
            dst = my_rank - current_comm_size/2;
            MPI_Send(&local_sum, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
        }

        // need to get messege
        else{

            // destination is my_rank + current_comm_size/2 
            dst = my_rank + current_comm_size/2;
            
            // but when destination is out of actual size, it should not expect to receive messege.
            if(dst <= comm_sz - 1){
                MPI_Recv(&receive_data, 1, MPI_INT, my_rank + current_comm_size/2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_sum += receive_data;
            }
        }

        current_comm_size /= 2;
    }

    // total sum is 0 process's total sum
    if(my_rank == 0) total_sum = local_sum;

// second, broacast total sum in all process:
    current_comm_size = 2;

    // only in this time, should this process be involed in
    while(my_rank + 1 > current_comm_size ) current_comm_size *= 2;

    while(current_comm_size <= adapt_length){

        // it should receive data
        if(my_rank >= current_comm_size/2){
            dst = my_rank - current_comm_size/2;
            MPI_Recv(&total_sum, 1, MPI_INT, dst, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // else it should send data
        else{
            dst = my_rank + current_comm_size/2;

            // but when destination is out of actual size, it should not expect to receive messege.
            if(dst <= comm_sz - 1){
                MPI_Send(&total_sum, 1, MPI_INT, dst, 0, MPI_COMM_WORLD);
            }
        }

        // updata current_comm_size
        current_comm_size *= 2;
    }

    printf("process %d of %d > get total sum is: %d.\n", my_rank, comm_sz,total_sum); 

    MPI_Finalize();

    return 0;
}