#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define MASTER_RANK 0
#define INT_SIZE 1
#define MIN_SIZE 128
#define MAX_SIZE 10000000

typedef struct t_words_qtt {
    int total, less_than_six, others;
}t_words_qtt;

void count_words(t_words_qtt *words_qtt, char *str){
    int wordSize = 0;
    for(int i=0; str[i]!='\0'; i++){
        if(wordSize && (str[i] == ' ' || str[i] == '\n' || str[i+1] == '\0')){
            ++words_qtt->total;
            if(wordSize < 6){
                words_qtt->less_than_six++;
            } else {
                words_qtt->others++;
            }
            wordSize = 0;
        } else {
            wordSize++;
        }
    }
}

int main(int argc, char * argv[]){
    int my_rank;
    char *msg;
    int msg_tag = 0;
    int msg_size_tag = 1;
    int nprocess;
    char end_message[4] = {'e', 'n', 'd', '\0'};
    t_words_qtt words_qtt = {
        .less_than_six = 0,
        .total = 0,
        .others = 0
    };

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
            MPI_Recv(&received_message_size, INT_SIZE, MPI_INT, MASTER_RANK, msg_size_tag, MPI_COMM_WORLD, &status);
            received_message = malloc(received_message_size);
            MPI_Recv(received_message, received_message_size, MPI_CHAR, MASTER_RANK, msg_tag, MPI_COMM_WORLD, &status);
            if(!strcmp(received_message, end_message)){
                free(received_message);
                break;
            }
            count_words(&words_qtt, received_message);
            printf("[%d] A mensagem que recebi \n'%s'\nPossui: %d palavras totais, %d delas são menores que seis e %d são maiores ou igual a seis.\n", my_rank, received_message, words_qtt.total, words_qtt.less_than_six, words_qtt.others);
            words_qtt.less_than_six = 0;
            words_qtt.others = 0;
            words_qtt.total = 0;
            free(received_message);
        }
    } else {
        int msg_size = MIN_SIZE;
        msg = malloc(msg_size);
        FILE* ptr;
        int i=1;
        int sended_messages = 0;
        ptr = fopen("test.txt", "a+");
        if (NULL == ptr) {
            printf("file can't be opened \n");
        }
        while(fgets(msg, msg_size, ptr) != NULL){
            MPI_Send(&msg_size, INT_SIZE, MPI_INT, i, msg_size_tag, MPI_COMM_WORLD);
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