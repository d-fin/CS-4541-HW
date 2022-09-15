#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

typedef struct CacheLine{
    int set;
    int valid;
    int tag;
    char *block1;
    char *block2;
}LINE;

int parseInput(int, char**, LINE ***);

int main(int argc, char *argv[]){
    
    if(argc != 9){
        fprintf(stderr, "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
        exit(-1);
    } 

    LINE **cache = NULL;  
    int result = parseInput(argc, **argv, &cache);
    
    return 0;
}

int parseInput(int argLength, char **input, LINE ***cache){
    /* 
    command line input 
    [-hv] -s <s> -E <E> -b <b> -t <tracefile> 
    */

    for(int i = 0; i < strlen(*input); i++){
        printf("%s\n", *input);
        input++;
    }

    return 0;
}