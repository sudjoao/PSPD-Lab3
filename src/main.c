#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define MASTER_RANK 0
#define MESSAGE_SIZE 100
#define MIN_SIZE 128
#define MAX_SIZE 10000000

int main(int argc, char * argv[]){
    int my_rank;
    char *msg;
    int tag = 0;
    int nprocess;
    MPI_Status status;
    // Inicia o MPI
    MPI_Init(&argc, &argv);

    // Define um rank para o MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);


    if(my_rank != MASTER_RANK){

    } else {
        int msg_size = MIN_SIZE;
        msg = malloc(msg_size);
        FILE* ptr;

        ptr = fopen("test.txt", "a+");
        if (NULL == ptr) {
            printf("file can't be opened \n");
        }
        while(fgets(msg, msg_size, ptr) != NULL){
            printf("%s\n", msg);
            free(msg);
            msg = malloc(++msg_size);
            if(msg_size == MAX_SIZE){
                msg_size = MIN_SIZE - 1;
            }
        }
        free(msg);
        fclose(ptr);
    }

    // Finaliza o MPI
    MPI_Finalize();

    return 0;
}