#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

int MASTER_RANK = 0;
int TAG_1 = 0;
int TAG_2 = 1;
int TAG_3 = 2;
int MAX_OFFSET_SIZE = 10000000;
int CHUNK_SIZE_OFFSET = 128;

// compilar: mpicc <nome_arquivo>
// executar: mpirun -n <numero_instancias> <caminho_do_binario_executável>

void execMaster();
void execWorker();
void sendEndSignToWorkers();
void sendMessageToWorker(char*, int, int);
void countWords(char*, int, int*);
void getResponse();
void updateRecieverRank(int*);
int isEndResponse(int*);
void updateChunkSize(int *);

int main(int argc, char * argv[]){
    int process_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);


    if(process_rank == MASTER_RANK){
        execMaster();
        return 0;
    }

    execWorker();
    return 0;
}


void execMaster(){
    clock_t begin = clock();

    // reading file
    FILE *file = fopen("file.txt", "r");
    if (file == NULL) exit(0);
    int chunkSize = CHUNK_SIZE_OFFSET;

    char *buffer = malloc(sizeof(char) * chunkSize);
    char c;
    int n = 0;

    int recieverRank = 1;

    while ((c = fgetc(file)) != EOF){
        if(n >= chunkSize){
            buffer[n] = '\0';
            sendMessageToWorker(buffer, chunkSize, recieverRank);
            updateRecieverRank(&recieverRank);
            updateChunkSize(&chunkSize);

            memset(buffer, 0, sizeof(*buffer)); // cleaning the data info
            buffer = realloc(buffer, chunkSize * sizeof(char)); // increasing the buffer size
            n = 0;
            continue;
        }
        buffer[n++] = c;
    }

    free(buffer);
    fclose(file);
    sendEndSignToWorkers();

    recieverRank = 1;
    getResponse();
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Exec time %.2f\n", time_spent);
    MPI_Finalize();
}

void updateChunkSize(int * chunkSize){
    if(*(chunkSize) >= MAX_OFFSET_SIZE){
        *(chunkSize) = CHUNK_SIZE_OFFSET;
        return;
    }
    *(chunkSize)+= CHUNK_SIZE_OFFSET;
}


void getResponse(){
     int temp[3] = {1,1,1};
     int res[3] = {0,0,0};
     int endCount = 0;

    int nprocess;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
    int nWorkers = nprocess - 1;

     while(endCount < nWorkers){
        MPI_Recv(temp, 3, MPI_INT, MPI_ANY_SOURCE, TAG_3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(isEndResponse(temp)){
            endCount++;
        }
        res[0]+= temp[0];
        res[1]+= temp[1];
        res[2]+= temp[2];
     }
    printf("Less than 6: %d \n", res[0]);
    printf("More than 6 and Less than 10: %d \n", res[1]);
    printf("More than 10: %d \n", res[2]);
}

int isEndResponse(int* res){
  return res[0] == 0 && res[1] == 0 && res[2] == 0;
}

void sendEndSignToWorkers(){
    int end = 0;
    char end_c = ' ';
    int nprocess;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
    for(int i = 1; i < nprocess; i++){
        MPI_Send(&end_c, 0, MPI_CHAR, i, TAG_2, MPI_COMM_WORLD);
    }
}

void sendMessageToWorker(char* msg, int msgSize, int recieverRank){
    // Getting the number of processes
    MPI_Send(msg, msgSize, MPI_CHAR, recieverRank, TAG_2, MPI_COMM_WORLD);
}

void updateRecieverRank(int* recieverRank){
    int nprocess;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
    if(*(recieverRank) == (nprocess - 1)){
        *(recieverRank) = 1;
        return;
    }
    *(recieverRank) = *(recieverRank) + 1;
}

void execWorker(){

    int response[3] = { 0, 0, 0 }; // less than 6, more than 6 char and less than 10, more than 10

    MPI_Status status;

    int msgSize;
    MPI_Probe(MASTER_RANK, TAG_2, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_CHAR, &msgSize);
    char * msg = malloc(sizeof(char) * msgSize);
    MPI_Recv(msg, msgSize, MPI_CHAR, MASTER_RANK, TAG_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    while(1){

        MPI_Probe(MASTER_RANK, TAG_2, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &msgSize);
        if(!msgSize) break;
        memset(msg, 0, sizeof(*msg));
        msg = realloc(msg, msgSize * sizeof(char));
        MPI_Recv(msg, msgSize, MPI_CHAR, MASTER_RANK, TAG_2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        countWords(msg, msgSize,response);
        MPI_Send(response, 3, MPI_INT, MASTER_RANK, TAG_3, MPI_COMM_WORLD);
        memset(response, 0, sizeof(response));
    }

    int endResponse[3] = {0,0,0};
    MPI_Send(endResponse, 3, MPI_INT, MASTER_RANK, TAG_3, MPI_COMM_WORLD);

    free(msg);
}

void countWords(char * msg, int msgSize, int * res){
    int count = 0;
    for(int i = 0; i < msgSize; i++){
        if(msg[i] != ' '){
            count++;
            continue;
        }
        if(i > 0 && msg[i] == ' ' && msg[i - 1] != ' '){
            if(count < 6) res[0]++;
            if(count > 6 && count < 10) res[1]++;
            if(count >= 10) res[2]++;
            count = 0;
        }
    }
}