#include "util.h"

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