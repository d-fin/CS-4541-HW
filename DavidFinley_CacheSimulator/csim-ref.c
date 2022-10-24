#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

/*
Command - -s 4 -E 1 -b 4 -t traces/yi.trace

L = load data 
S = store data 
M = modify data (data load then data store)
I = instruction load 
 */

typedef struct cache_line{
    int valid;
    int tag;
    char *block;
    int num_in_q;
}LINE;

typedef struct Cache{
    LINE **line;
}CACHE;

typedef struct Commands{
    char *commandType;
    char *address;
    int size;
}COMMANDS;

int load(char *, CACHE **);
int store(char *, CACHE **);
int instructionLoad(CACHE *, COMMANDS ***, int *);
int parseFile(char *, COMMANDS ***, int *);
void printCommands(COMMANDS ***, int *);
int decToBinary(char *, char **);

int main(int argc, char *argv[]){
    // Harcoded for now 
    int S = 4; 
    int E = 1; 
    int B = 4;
    char *file_name = "traces/yi.trace";
    //************************************
    COMMANDS **commands = NULL;
    int n_commands = 0;
    int verbose = 0;
    int hits, misses, evictions = 0;
    int q[S];

    /* if(argc > 10 || argc < 9){
        fprintf(stderr, "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
        exit(-1);
    }  
    else{
        if(argc == 9){
            S = atoi(argv[2]);
            E = atoi(argv[4]);
            B = atoi(argv[6]);
            file_name = argv[8];
        }
        else{
            verbose = 1;
            S = atoi(argv[3]);
            E = atoi(argv[5]);
            B = atoi(argv[7]);
            file_name = argv[9];
        }
       
        //printf("sets - %d\nlines per set - %d\nblock offset bits - %d\nfile name - %s\n", sets, lines, blockOffsetBits, file_name);
    } */

    /*
        Parsing the trace file and storing the commands in struct commands.
    */
    int result = parseFile(file_name, &commands, &n_commands);
    if(result != 0) return -1;

    /*
        Allocating memory for the cache and the cache lines based on S and E vals. 
    */
    CACHE *cache = malloc(sizeof(CACHE));
    cache->line = (LINE **)malloc(S * sizeof(LINE *));
    for (int i = 0; i < S; i++){
        cache->line[i] = (LINE *)malloc(E * sizeof(LINE));
        for (int j = 0; j < E; j++){
            cache->line[i][j].valid = 0;
            cache->line[i][j].tag = 0;
            cache->line[i][j].block = "";
            cache->line[i][j].num_in_q = 0;
        }
    }
    for (int i = 0; i < S; i++){
        for (int j = 0; j < E; j++){
            printf("\n\nSET # %d - LINE # %d", i, j);
            printf("\nvalid - %d",cache->line[i][j].valid);
            printf("\ntag - %d",cache->line[i][j].tag);
            printf("\nblock - %s",cache->line[i][j].block);
            printf("\nnum in q - %d",cache->line[i][j].num_in_q);
        }
    }
    /* 
        Loop through commands 
    */
    int z = 0;
    while (z < n_commands){
        for (int command_num = 0; command_num < n_commands; command_num++){
            printf("\nCommand: %s\nAddress: %s", commands[command_num]->commandType, commands[command_num]->address);
            int n = atoi(commands[command_num]->address); 
            int len = 0;
            int binary[32];
            char *bin; 

            /* convert address to binary string */
            while (n > 0) {
                binary[len] = n % 2;
                int temp = n % 2;
                char temp2[2];
                sprintf(temp2, "%d", temp);
                strcat(bin, temp2);
                n = n / 2;
                len++;
            }
            
            /* determine tag, block bits */
            int tagBits = len - log2(B) - E;

            if (strcmp(commands[command_num]->commandType, "L") == 0){
                int found = 0;
                for (int i = 0; i < S; i++){
                    for (int j = 0; j < E; j++){
                        if (cache->line[i][j].valid == 1){
                            printf("\n\nvalid");
                        }
                    }
                    /* if (cache->line == 1){
                        char *tag;
                        for (int c = 0; c < tagBits; c++){
                            strcat(tag, bin[c]);
                        }
                    }    */
                    
                } 
                if (found == 0){
                    /* store in q */ 
                    for (int i = 0; i < S; i++){
                        for (int j = 0; j < E; j++){
                           
                        }
                    }
                }
            }
            else if (strcmp(commands[command_num]->commandType, "S") == 0){
            }
        }
        z++;
    }

    printf("\nhits: %d misses: %d evictions: %d\n ", hits, misses, evictions);
    return 0;
}

/* load the instruction from the cache */
int load(char *address, CACHE **cache){
    return 0;
}

/* store the instruction in the cache */
int store(char *address, CACHE *cache[]){
    return 0;
}

/*  */
int instructionLoad(CACHE *cache, COMMANDS ***commands, int *n_commands){
    int i = 0; 
    
    return 0;
}

/* Parse the provided trace file. */ 
int parseFile(char *file_name, COMMANDS ***commands, int *n_commands){
    FILE *file = fopen(file_name, "r");
    if(errno == ENOENT){
        fprintf(stderr, "./parser: cannot open(\"%s\"): No such file or directory\n", file_name);
		return -1;
    }

    char *buffer = NULL;
    size_t n = 0;
    int result = 0;
    int x_commands = * n_commands;

    while((result = getline(&buffer, &n, file)) != -1 ){
        *commands = realloc(*commands, sizeof(COMMANDS) * (x_commands + 1));
        (*commands)[x_commands] = malloc(sizeof(COMMANDS));
        COMMANDS *this = (*commands)[x_commands];
        char *stringx = buffer;

        stringx++;
        char *commandType = strdup(strsep(&stringx, " "));
        char *address = strdup(strsep(&stringx, ","));
        int size = strtol(strsep(&stringx, "\\"), NULL, 10);

        this->commandType = commandType;
        this->address = address;
        this->size = size;
        
        x_commands++;
    }
    *n_commands = x_commands;
    free(buffer);
    fclose(file);

    return 0;
}

/*int decToBinary(char *x, char **bin){
    int n = atoi(x);
    int binaryNum[32];
    int len = 0;
    while (n > 0) {
        binaryNum[len] = n % 2;
        **bin = n % 2;
        n = n / 2;
        len++;
        
    }
    
    int bin[len];
    int k = 0;
    for (int i = 0; i < len; i++){
        k = 10 * k + binaryNum[i];
        int temp = binaryNum[i];
        bin[i] = temp;
    }
        
    return bin; 
}*/
