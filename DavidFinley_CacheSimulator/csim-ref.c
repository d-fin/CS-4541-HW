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
    char *tag;
    char *block;
    int num_in_q;
}LINE;

typedef struct Cache{
    LINE **line;
    int in_q;
}CACHE;

typedef struct Commands{
    char *commandType;
    char *address;
    int size;
}COMMANDS;

int load(char *, CACHE *, int, int, int);
int store(char *, CACHE *, int, int, int);
int parseFile(char *, COMMANDS ***, int *);
void printCommands(COMMANDS ***, int *);
int evict_FIFO(CACHE *, int, int);
char* toBin(char *, int **);

int main(int argc, char *argv[]){
    int S = 0;
    int E = 0;
    int B = 0;
    char *file_name = NULL;
    COMMANDS **commands = NULL;
    int n_commands = 0;
    int verbose = 0;
    int hits = 0;
    int misses = 0;
    int evictions = 0;
    

    if(argc > 10 || argc < 9){
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
    } 

    /*
        Parsing the trace file and storing the commands in struct commands.
    */
    int result = parseFile(file_name, &commands, &n_commands);
    if(result != 0) return -1;

    /*
        Allocating and initializing memory for the cache and the cache lines based on S and E vals. 
    */
    CACHE *cache = malloc(sizeof(CACHE));
    cache->in_q = 0;
    cache->line = (LINE **)malloc(S * sizeof(LINE *));
    for (int i = 0; i < S; i++){
        cache->line[i] = (LINE *)malloc(E * sizeof(LINE));
        for (int j = 0; j < E; j++){
            cache->line[i][j].valid = 0;
            cache->line[i][j].tag = "";
            cache->line[i][j].block = "";
            cache->line[i][j].num_in_q = 0;
        }
    }
    
    /* 
        Loop through commands 
    */
    for (int command_num = 0; command_num < n_commands; command_num++){
        int load_res, store_res = 0;
        printf("%s %s,%d ", commands[command_num]->commandType, commands[command_num]->address, commands[command_num]->size);
        if (strcmp(commands[command_num]->commandType, "L") == 0){
            load_res = load(commands[command_num]->address, cache, S, E, B);
            if (load_res == 0){
                misses++;
                printf("miss ");
                store_res = store(commands[command_num]->address, cache, S, E, B);
                if (store_res == 1){
                    evictions++;
                    printf("eviction ");
                }
            }
            else{
                hits++;
                printf("hit");
            }
        }
        else if (strcmp(commands[command_num]->commandType, "S") == 0){
            store_res = store(commands[command_num]->address, cache, S, E, B);
            if (store_res == 1){
                evictions++;
                printf("eviction ");
            }
            else{
                hits++;
                printf("hit ");
            }
        }
        else if (strcmp(commands[command_num]->commandType, "M") == 0){
            load_res = load(commands[command_num]->address, cache, S, E, B);
            if (load_res == 0){
                misses++;
                printf("miss ");
            }
            else{
                hits++;
                printf("hit ");
            }

            store_res = store(commands[command_num]->address, cache, S, E, B);
            if (store_res == 1){
                evictions++;
                printf("eviction ");
                printf("hit ");
                hits++;
            }
            else{
                hits++;
                printf("hit ");
            }
            
        }
        printf("\n");
    }
       
    printf("\nhits: %d misses: %d evictions: %d\n ", hits, misses, evictions);
    
    /* free memory */
    for (int i = 0; i < S; i++){
        for(int j = 0; j < E; j++){
            free(cache->line[i][j].tag);
        }
        free(cache->line[i]);
    } 
    free(cache->line);
    free(cache);
    for (int i = 0; i < n_commands; i++){
        free(commands[i]->address);
        free(commands[i]->commandType);
        free(commands[i]);
    }
    free(commands);
    return 0;
}

/* load the instruction from the cache */
int load(char *address, CACHE *cache, int S, int E, int B){

    int base2 = 0;
    int n;

    for (int i = 0; i < strlen(address); i++){ 
        if (address[i] >= '0' && address[i] <= '9'){
            base2 = 0;
        }
        else{
            base2 = 1;
        }
    }
    if (base2 == 0){
        n = atoi(address); 
    }
    else{
        n = strtol(address, NULL, 16);
    }
    
    int len = 0;
    int binary[32];
    char bin[10]; 

    /* convert address to binary string */
    while (n > 0) {
        binary[len] = n % 2;
        int temp = n % 2;
                /* char *temp2;
                sprintf(temp2, "%d", temp);
                strcat(bin, temp2); */
        n = n / 2;
        len++;
    }
    bin[len] = '\0';
    /* convert binary to string */
    int t = 0;
    for (int i = 0; i < len; i++){
        t += sprintf(&bin[i], "%d", binary[i]);
    }
    char *binCopy = strndup(bin, len);
    binCopy[len] = '\0';
    /* determine tag, block bits */
    int tagBits = len - (S - (log(B)/log(2)));
    if (tagBits <= 0){
        tagBits = 1;
    }


    char *target = strndup(bin, tagBits);

    //printf("\n\nLOAD :\nBin_String - %s\nTag bits - %d\nTag - %s\n\n", bin, tagBits, target);
    
    for (int i = 0; i < S; i++){
        for (int j = 0; j < E; j++){
            if (cache->line[i][j].valid == 1){
                if (strcmp(cache->line[i][j].tag, target) == 0){
                    free(binCopy);
                    free(target);
                    return 1;
                }
            }
        }
    } 

    free(target);
    free(binCopy);
    return 0;
}

/* store the instruction in the cache */
int store(char *address, CACHE *cache, int S, int E, int B){
    int base2 = 0;
    int n;

    for (int i = 0; i < strlen(address); i++){
        if (address[i] >= '0' && address[i] <= '9'){
            base2 = 0;
        }
        else{
            base2 = 1;
        }
    }
    if (base2 == 0){
        n = atoi(address); 
    }
    else{
        n = strtol(address, NULL, 16);
    }
    
    int len = 0;
    int binary[32];
    char bin[len]; 

    /* convert address to binary string */
    while (n > 0) {
        binary[len] = n % 2;
        int temp = n % 2;
        n = n / 2;
        len++;
    }
    bin[len] = '\0';
    /* convert binary to string */
    int t = 0;
    for (int i = 0; i < len; i++){
        t += sprintf(&bin[i], "%d", binary[i]);
    }
    char *binCopy = strdup(bin);
    binCopy[len] = '\0';

    /* determine tag, block bits */
    int tagBits = len - (S - (log(B)/log(2)));
    if (tagBits <= 0){
        tagBits = 1;
    }
    //printf("\ntag bits - %d\n", tagBits);
    char *target = strndup(binCopy, tagBits);
    //printf("\n\nSTORE :\nBin_String - %s\nTag bits - %d\nTag - %s\n\n", bin, tagBits, target);
    

    int evicted = 0;
    if (cache->in_q >= S){
        evicted = evict_FIFO(cache, S, E);
    }
    
    int in_cache = 0;
    for (int i = 0; i < S; i++){
        for (int j = 0; j < E; j++){
            if (cache->line[i][j].valid == 0 && in_cache == 0){
                cache->line[i][j].valid = 1;
                cache->line[i][j].tag = strdup(target);
                cache->line[i][j].num_in_q++;
                in_cache = 1;
                cache->in_q++;
            }
            else if (cache->line[i][j].valid != 0){
                cache->line[i][j].num_in_q++;
            }
        }
    }
    
    free(target);
    free(binCopy);
    if (evicted != 0){
        return 1;
    }
    else{
        return 0;
    }
}

int evict_FIFO(CACHE *cache, int S, int E){
    for (int i = 0; i < S; i++){
        for (int j = 0; j < E; j++){
            if (cache->line[i][j].num_in_q == 4){
                cache->line[i][j].valid = 0;
                //free(cache->line[i][j].tag);
                //cache->line[i][j].tag = "";
                cache->line[i][j].num_in_q = 0; 
            }
            else{
                cache->line[i][j].num_in_q++;
            }
        }
    }
    return 1;
}

/* Parse the provided trace file. */ 
int parseFile(char *file_name, COMMANDS ***commands, int *n_commands){
    FILE *file = fopen(file_name, "r");
    if(errno == ENOENT){
        fprintf(stderr, "./csim-ref: cannot open(\"%s\"): No such file or directory\n", file_name);
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
        if (*stringx != 'I'){
            stringx++;
            this->commandType = strdup(strsep(&stringx, " "));
            this->address = strdup(strsep(&stringx, ","));
            this->size = strtol(strsep(&stringx, "\\"), NULL, 10);
            x_commands++;
        }
        else{
            free(this);
        }
        
    }
    *n_commands = x_commands;
    free(buffer);
    fclose(file);

    return 0;
}
