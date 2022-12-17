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
    return send_recv_algorithm(argc, argv);
}