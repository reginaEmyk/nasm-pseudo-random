#include <stdio.h>

__int32_t _lfsr();

int main(){
    // printf("Hello, World!");
    
    int i = _lfsr();
    printf("%i ", i);
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