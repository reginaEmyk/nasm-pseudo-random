#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    #define _STARTING_BIT_ 0x800000// only the last 24 bits are 
    #define _LAST_BIT_ 0x000001//
    #define _24_BITS_ 0xFFFFFF// only the last 24 bits are 1
    #define _BIT_MASK_TAPS_ 0x00001B // mask where only TAPS are one (24,22,21,20), big endian for cases like seed = 1
    #define array_size 16777216


int get_polynomial(int lfsr){
    int bit, aux;

    bit = (lfsr & _BIT_MASK_TAPS_);
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & _LAST_BIT_;
    bit = (bit << 23); // todo checl if this ever above 24

    if (bit >= array_size){
        printf("bit is bigger than array size");
        exit(1);
    }

    return bit;
}


int* lfsr_array(int* pseudoRandom, int lfsr);
int* lfsr_array(int* pseudoRandom, int lfsr){
    int polynomial = -1;

    for (int i = 0; i < array_size-1; i++){
        polynomial = get_polynomial(lfsr);
        lfsr = (lfsr >> 1) | polynomial;

        pseudoRandom[i] = lfsr;
    }
    
    return pseudoRandom;
 
}



int* _lfsr(__int32_t);

int comp (const void * elem1, const void * elem2) 
{
    int f = *((int*)elem1);
    int s = *((int*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}

int main(){
    // printf("Hello, World!");
    // 16777216
    int* pseudoRandom = (int*)malloc(array_size * sizeof(int));
    int* CPseudoRandom = (int*)malloc(array_size * sizeof(int));
    int* pseudR_frequency = (int*)malloc(array_size * sizeof(int));
    int count_ocurr = 0;
    double freq_ob = 0; 
    double freq_exp = 1/array_size;
    double one_freq = freq_exp;
    double chi_sqr = 0;
    time_t time_nasm, time_c;
    clock_t clock_nasm, clock_c ;  



    printf("calling 16777215 \n;");
    time_nasm = time(NULL);
    clock_nasm = clock();
    pseudoRandom = _lfsr(1);
    time_nasm = time(NULL) - time_nasm;
    clock_nasm = clock() - clock_nasm;

    time_c = time(NULL);
    clock_c = clock();
    CPseudoRandom = lfsr_array(CPseudoRandom, 1);
    clock_c = clock() - clock_c;
    time_c = time(NULL) - time_c;

    // for (int i = 0; i < array_size; i++)
    // {
    //     printf(" %i; ", pseudoRandom[i]);
    // }
    
    printf("\n time taken by nasm: %ld clocks, %ld secs", clock_nasm, clock_nasm/CLOCKS_PER_SEC);
    printf("\n time taken by c: %ld clocks, %ld secs \n", clock_c, clock_c/CLOCKS_PER_SEC);



    // printf("\n finished printing 16777215");
    // for (int i = 0; i < array_size; i++)
    // {
    //     printf(" %i; ", pseudoRandom[i]);
    // }

    // printf("\n getting chi_sqr index");
    // // getting chi_sqr index
    // qsort(pseudoRandom, array_size, sizeof(int), comp);

    // printf("\n sorted array");
    // for (int i = 0; i < array_size; i++)
    // {
    //     if(i < array_size - 1 && pseudoRandom[i] == pseudoRandom[i+1] ){
    //         for (count_ocurr = 1; count_ocurr <= array_size, pseudoRandom[i] == pseudoRandom[i+1] ; count_ocurr++, i++);
    //         chi_sqr += (freq_ob - freq_exp )*(freq_ob - freq_exp )/(freq_exp);
    //     } else{
    //         chi_sqr += (1 - freq_exp )*(freq_ob - freq_exp )/(freq_exp);
    //     }
    // }
    // printf("\n chi_sqr index: %f\n", chi_sqr);
    
    

    //chi square i sum of (realFreq - expFreq)**2/(expFreq)
   

    // todo: make a huge 2^24 -1 vector calling lfsr
    // todo: chi test it

    // Set of unsigned 24 bit numbers: from 1 to 16777215, 
    // each is expected to happen 1/16777215 times

    // 1M is 1048576 = 2^20

    // any number in any class is expected to happen 1/2^20 times per 1M class

    // generate 16M numbers
    // sort
    // get frequency for all classes ( (0, 1M], (1M,2M]... )
        // Expected frequency of each number is 1/1M
        // Observed freq is <n ocurrences>/1M
    // Calculate chi squared estimator

    
    return 0;
}