#include "util.h"
#include "send_recv.h"

#define MASTER_RANK 0
#define INT_SIZE 1
#define MIN_SIZE 128
#define MAX_SIZE 4000
#define TOTAL_TAG 3
#define LESS_THAN_SIX_TAG 4
#define OTHERS_TAG 5



int main(int argc, char * argv[]){
    int returned_value;

    if(argc < 2){
        printf("Você deve passar o número do algoritmo que deseja rodar como argumento\n");
        return 1;
    }

    if(argv[1][0] == '1'){
        returned_value =  send_recv_algorithm(argc, argv);
    } else {
        printf("%s\n", argv[1]);
    }
    return returned_value;
}