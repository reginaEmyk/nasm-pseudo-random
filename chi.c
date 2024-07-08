#include <stdio.h>
#include <stdlib.h>

__int32_t _lfsr(__int32_t);

int main(){
    // printf("Hello, World!");
    // 16777216
    __uint32_t* pseudoRandom = (__uint32_t*)malloc(16777216 * sizeof(__uint32_t));
    printf("calling 16777215 \n;");
    for (int i = 0; i < 16777; i++)
    {
        if(i==0)
            pseudoRandom[i] = _lfsr(0);
        else
            pseudoRandom[i] = _lfsr(pseudoRandom[i-1]);
        printf(" %i; ", pseudoRandom[i]);
    }
    printf("\n finished printing 16777215");

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