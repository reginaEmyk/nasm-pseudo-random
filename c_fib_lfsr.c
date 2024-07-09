#include <stdio.h>
#include <stdlib.h>
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


int* lfsr_array(int* pseudoRandom, int seed);
int* lfsr_array(int* pseudoRandom, int seed){
    int lfsr = seed;
    int polynomial = -1;

    for (int i = 0; i < array_size; i++){
        polynomial = get_polynomial(lfsr);
        lfsr = (lfsr >> 1) | polynomial;

        pseudoRandom[i] = lfsr;
    }
    
    return pseudoRandom;
 
}


int main(){
    int* pseudoRandom = (int*)malloc(16777216 * sizeof(int));
    
    pseudoRandom = lfsr_array(pseudoRandom, 1);
    for (int i = 0; i < array_size; i++)
    {
        printf("%i ", pseudoRandom[i]);
    }
    // todo: make a huge 2^24 -1 vector calling lfsr
    // todo: chi test it
    
    return 0;
}