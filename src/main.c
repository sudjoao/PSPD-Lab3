#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define MASTER_RANK 0
#define INT_SIZE 1
#define MIN_SIZE 128
#define MAX_SIZE 10000000

int main(int argc, char * argv[]){
    int my_rank;
    char *msg;
    int msg_tag = 0;
    int msg_size_tag = 1;
    int nprocess;
    char end_message[4] = {'e', 'n', 'd'};
    end_message[4] = '\0';
    MPI_Status status;
    // Inicia o MPI
    MPI_Init(&argc, &argv);

    // Define um rank para o MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);


    if(my_rank != MASTER_RANK){
        int received_message_size;
        char *received_message;
        while(1){
            printf("[%d]Aguardando mensagem de tamanho\n", my_rank);
            MPI_Recv(&received_message_size, INT_SIZE, MPI_INT, MASTER_RANK, msg_size_tag, MPI_COMM_WORLD, &status);
            printf("[%d]Aguardando mensagem com conteudo\n", my_rank);
            received_message = malloc(received_message_size);
            MPI_Recv(received_message, received_message_size, MPI_CHAR, MASTER_RANK, msg_tag, MPI_COMM_WORLD, &status);
            printf("Processo %d enviou %d caracteres:\n'%s'\n", status.MPI_SOURCE, received_message_size, received_message);
            if(!strcmp(received_message, end_message)){
                break;
            }
            free(received_message);
        }
    } else {
        int msg_size = MIN_SIZE;
        msg = malloc(msg_size);
        FILE* ptr;
        int i=1;
        ptr = fopen("test.txt", "a+");
        if (NULL == ptr) {
            printf("file can't be opened \n");
        }
        while(fgets(msg, msg_size, ptr) != NULL){
            // printf("[%d]Enviando mensagem de tamanho\n", my_rank);
            MPI_Send(&msg_size, INT_SIZE, MPI_INT, i, msg_size_tag, MPI_COMM_WORLD);
            // printf("[%d]Enviando mensagem com conteudo\n", my_rank);
            MPI_Send(msg, msg_size++, MPI_CHAR, i, msg_tag, MPI_COMM_WORLD);
            free(msg);
            msg = malloc(msg_size);
            i++;
            if(i == nprocess){
                i = 0;
            }
            if(msg_size == MAX_SIZE){
                msg_size = MIN_SIZE;
            }
        }
        free(msg);
        fclose(ptr);
        msg_size = 4;
        for(i=0; i<nprocess; i++){
            MPI_Send(&msg_size, INT_SIZE, MPI_INT, i, msg_size_tag, MPI_COMM_WORLD);
            MPI_Send(end_message, msg_size, MPI_CHAR, i, msg_tag, MPI_COMM_WORLD);
        }
    }

    // Finaliza o MPI
    MPI_Finalize();

    return 0;
}