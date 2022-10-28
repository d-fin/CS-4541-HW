#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


typedef struct Line{
    unsigned long long block;
    short valid;
    unsigned long long tag;
    int r_rate;
    int p_rate;
}LINE;

typedef struct Set{
    LINE *lines;
    int rr;
    int pr;
}SET;

typedef struct Cache{
    int set_bits;
    int associativity;
    int block_bits;
    SET *sets;
    int evictions;
    int hits;
    int misses;
    int verbose;
}CACHE;

unsigned long long in_cache(unsigned long long addres, CACHE *);
void evict_FIFO(unsigned long long address, CACHE *);
void run_cache(unsigned long long address, CACHE *);

int main(int argc, char *argv[]){
    CACHE cache;
    char *file_name;
    
    if(argc > 10 || argc < 9){
        fprintf(stderr, "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
        exit(-1);
    }  

    else{
        if(argc == 9){
            cache.set_bits = atoi(argv[2]);
            cache.associativity = atoi(argv[4]);
            cache.block_bits = atoi(argv[6]);
            cache.verbose = 0;
            file_name = argv[8];
        }
        else{
            cache.verbose = 1;
            cache.set_bits = atoi(argv[3]);
            cache.associativity = atoi(argv[5]);
            cache.block_bits = atoi(argv[7]);
            file_name = argv[9];
        }       
    } 

    cache.hits = 0;
    cache.evictions = 0;
    cache.misses = 0;
    unsigned long long set = pow(2, cache.set_bits);
    cache.sets = (SET *)malloc(sizeof(SET)*set);
    for(unsigned long long i=0; i < set; i++)
    {
        cache.sets[i].rr = 0;
        cache.sets[i].pr = 0;
        cache.sets[i].lines = (LINE *)malloc(sizeof(LINE)*cache.associativity);
        for(unsigned long long j=0; j < cache.associativity; j++)
        {
            cache.sets[i].lines[j].valid = 0;
        }
    }
    FILE *in_file = fopen(file_name, "r");
    
    char *command;
    unsigned long long address;
    int size;

    while(fscanf(in_file, " %s %llx,%d", command, &address, &size) == 3){
        if(cache.verbose)
            printf("%s %llx,%d", command, address, size);
        
        if (strcmp(command, "L") == 0){
            run_cache(address, &cache);
        }
        if (strcmp(command, "S") == 0){
            run_cache(address, &cache);
        }
        else if (strcmp(command, "M") == 0){
            run_cache(address, &cache);
            cache.hits++;
            if(cache.verbose)
                printf(" hit");
        }
        if(cache.verbose)
            printf("\n");
    }
    fclose(in_file);

    printf("hits:%d misses:%d evictions:%d\n", cache.hits, cache.misses, cache.evictions);
    
    return 0;
}

unsigned long long in_cache(unsigned long long address, CACHE *cache){

    unsigned long long index = 0;
    unsigned long long tagBits = 64 - cache->set_bits - cache->block_bits;
    unsigned long long tag = address >> (64 - tagBits);
    unsigned long long set = (address << tagBits) >> (64 - cache->set_bits);
    
    for(unsigned long long i=0; i < cache->associativity; i++)
        if(cache->sets[set].lines[i].valid && cache->sets[set].lines[i].tag == tag){
            cache->sets[set].lines[i].r_rate = ++cache->sets[set].rr;

            if(cache->verbose)
                printf(" hit");

            return 1;
        }
        else if(!cache->sets[set].lines[i].valid){
            cache->sets[set].lines[index].tag = tag;
            cache->sets[set].lines[index].valid = 1;
            cache->sets[set].lines[index].r_rate = ++cache->sets[set].rr;
            cache->sets[set].lines[index].p_rate = ++cache->sets[set].pr;
            
            return 2;
        }

    return 0;
}

void evict_FIFO(const unsigned long long address, CACHE *cache){
    unsigned long long tagBits = 64 - cache->set_bits - cache->block_bits;
    unsigned long long tag = address >> (64 - tagBits);
    unsigned long long set = (address << tagBits) >> (64 - cache->set_bits);
    unsigned long long index = 0;
    unsigned long long min;
    
    min = cache->sets[set].lines[0].p_rate;
    for(unsigned long long i=1; i < cache->associativity; i++){
        unsigned long long value;
        
        value = cache->sets[set].lines[i].p_rate;
        if(min > value){
            min = value;
            index = i;
        }
    }
    cache->sets[set].lines[index].tag = tag;
    //cache->sets[set].lines[index].r_rate = ++cache->sets[set].rr;
    //cache->sets[set].lines[index].p_rate = ++cache->sets[set].pr;
}


void run_cache(unsigned long long address, CACHE *cache){

    unsigned long long result = in_cache(address, cache);

    if(result == 1){
        cache->hits++;
    }
    else{
        cache->misses++;
        if(cache->verbose)
            printf(" miss");

        if(result == 0)
        {
            evict_FIFO(address, cache);
            cache->evictions++;

            if(cache->verbose)
                printf(" eviction");
        }
    }
}


