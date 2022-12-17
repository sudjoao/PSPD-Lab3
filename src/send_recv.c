#include "send_recv.h"


#define MAX_SIZE 4000

int send_recv_algorithm(int argc, char * argv[]){
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
            printf("%d\n", received_message_size);
            received_message = malloc(received_message_size);
            MPI_Recv(received_message, received_message_size, MPI_CHAR, MASTER_RANK, msg_tag, MPI_COMM_WORLD, &status);
            if(!strcmp(received_message, end_message)){
                free(received_message);
                break;
            }
            count_words(&words_qtt, received_message);
            MPI_Send(&words_qtt.total, INT_SIZE, MPI_INT, MASTER_RANK, TOTAL_TAG, MPI_COMM_WORLD);
            MPI_Send(&words_qtt.less_than_six, INT_SIZE, MPI_INT, MASTER_RANK, LESS_THAN_SIX_TAG, MPI_COMM_WORLD);
            MPI_Send(&words_qtt.others, INT_SIZE, MPI_INT, MASTER_RANK, OTHERS_TAG, MPI_COMM_WORLD);
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
        ptr = fopen("test.txt", "r");
        if (NULL == ptr) {
            printf("file can't be opened \n");
            return 1;
        }
        while(1){
            char ch;
            int j;
            for(j=0; j<msg_size;j++){
		        ch = fgetc(ptr);
		        if(ch == EOF){
                    break;
                }
                msg[j] = ch;
            }
            if(j==0 && ch==EOF)
                break;
            msg[j] = '\0';
            MPI_Send(&msg_size, INT_SIZE, MPI_INT, i, msg_size_tag, MPI_COMM_WORLD);
            MPI_Send(msg, msg_size++, MPI_CHAR, i, msg_tag, MPI_COMM_WORLD);
            free(msg);
            msg = malloc(msg_size);
            sended_messages++;
            i++;
            if(i == nprocess){
                i = 1;
            }
            if(msg_size == MAX_SIZE){
                msg_size = MIN_SIZE;
            }

            if(ch == EOF){
                break;
            }
        }

        free(msg);
        fclose(ptr);
        for(i=0; i<sended_messages; i++){
            int total, less_than_six, others;
            MPI_Recv(&total, INT_SIZE, MPI_INT, MPI_ANY_SOURCE, TOTAL_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&less_than_six, INT_SIZE, MPI_INT, MPI_ANY_SOURCE, LESS_THAN_SIX_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&others, INT_SIZE, MPI_INT, MPI_ANY_SOURCE, OTHERS_TAG, MPI_COMM_WORLD, &status);
            words_qtt.total+=total;
            words_qtt.less_than_six+=less_than_six;
            words_qtt.others+=others;
        }
        msg_size = 4;
        for(i=0; i<nprocess; i++){
            MPI_Send(&msg_size, INT_SIZE, MPI_INT, i, msg_size_tag, MPI_COMM_WORLD);
            MPI_Send(end_message, msg_size, MPI_CHAR, i, msg_tag, MPI_COMM_WORLD);
        }
        printf("O arquivo enviado possui: %d palavras totais, %d delas são menores que seis e %d são maiores ou igual a seis.\n", words_qtt.total, words_qtt.less_than_six, words_qtt.others);
    }


    // Finaliza o MPI
    MPI_Finalize();
    return 0;
}