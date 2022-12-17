#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#define MASTER_RANK 0
#define INT_SIZE 1
#define MIN_SIZE 128
#define TOTAL_TAG 3
#define MSG_TAG  0
#define LESS_THAN_SIX_TAG 4
#define OTHERS_TAG 5
#define MAX_SIZE 10000000

typedef struct t_words_qtt {
    int total, less_than_six, others;
}t_words_qtt;

void count_words(t_words_qtt *words_qtt, char *str);


#endif /* UTIL_H_ */