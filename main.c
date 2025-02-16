// to run
// nasm -g -f elf32 lfsr.asm # compile for 32 bits arch
// gcc -g -m32 main.c lfsr.o -o lfsr
// ./lfsr 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    #define STARTING_BIT 0x800000// only the last 24 bits are 
    #define LAST_BIT 0x000001//
    #define BITS_24 0xFFFFFF// only the last 24 bits are 1
    #define BIT_MASK_TAPS 0x00001B // mask where only TAPS are one (24,22,21,20), big endian for cases like seed = 1
    #define ARRAY_SIZE 16777215 // number of numbers to generate (16777215 is the biggest 24bit int)
    #define N_CLASSES 16
    int const CLASS_SIZE = (BITS_24+1)/N_CLASSES;
    double const UNIFORM_FREQ =  (double)1/CLASS_SIZE; // the frequency expected for an ocurrence in a class is 1/size of class, in uniform distribution 
    double const ALMOST_ZERO_DENOMINATOR = 0.1;

    // get the critical value in this table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf
    double const CRITICAL_VALUE = 25.00; // for alpha = 0.05 and df = N_classes -1 = 15
    #define SEED 1

int get_polynomial_a(int lfsr){
    int bit, aux;

    bit = (lfsr & BIT_MASK_TAPS);
 // bit =  x^24 + x^23        + x^21        + x^20         + 1
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & LAST_BIT;
    bit = (bit << 23);

    return bit;
}


int* lfsr_array(int* pseudoRandom, int lfsr);
int* lfsr_array(int* pseudoRandom, int lfsr){
    int polynomial = -1;

    for (int i = 0; i < ARRAY_SIZE; i++){
        polynomial = get_polynomial_a(lfsr);
        lfsr = (lfsr >> 1) | polynomial;

        pseudoRandom[i] = lfsr;
    }
    
    return pseudoRandom;
 
}

int* lfsr(int);

// null hypothesis: it is uniform, numbers [0, 1M) are expected to be in 1st class
double chiUniformExpected(int* pseudoRandom) {
    printf("\nInitiating chi square test... \n");
    double chi = 0;
    const int expectedFrequency = ARRAY_SIZE/N_CLASSES; // if array_size = 2^24, expectedFrequency = 1.048.576

    int* observationsPerClass = calloc(N_CLASSES,sizeof(int)); // 0 initialize at calloc

    for (int i = 0; i < ARRAY_SIZE; i++) // iterate through each "random" number and associating it to a class
    {
        observationsPerClass[pseudoRandom[i]/CLASS_SIZE]++;
    }


    printf("%15s %12s %8s  %8s\n", "Number in class", "", "Observed", "Expected");
    // Calculating chi
    for (int n = 0; n < N_CLASSES; n++) // 16 classes
    {
        printf("Class (%8d, %8d] : %7d | %7d\n", n*CLASS_SIZE, (n+1)*CLASS_SIZE, observationsPerClass[n], expectedFrequency);
        int a = (observationsPerClass[n] - expectedFrequency);
        chi += a*a;
    }
    chi /= expectedFrequency;

    free(observationsPerClass);
    return chi;
}


int main(){
    int* nasmPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    int* cPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    clock_t clockNasm, clockC ;  // function execution takes less than a second, must use clock in ANSI C. https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c.
    int areBothEqual = 0;
    int hasRepeated = 0;
    
    clockNasm = clock();
    printf("Calling LFSR in nasm, SEED: %i ...", SEED);
    nasmPseudoRandom = lfsr(SEED);

    printf("Finished\n");
    clockNasm = clock() - clockNasm;

    clockC = clock();
    printf("Calling LFSR in c, SEED: %i ...", SEED);
    cPseudoRandom = lfsr_array(cPseudoRandom, SEED);
    printf("Finished\n");
    clockC = clock() - clockC;

// A test to see if 0 was generated, in which case the next number would be 0.
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if(cPseudoRandom[i] == 0)
            printf("ZERO WAS GENERATED! cPseudoRandom[%i] is 0 \n", i);
    }

// check if both implementations produced the same values in the same order
// both given same seed
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if(nasmPseudoRandom[i] != cPseudoRandom[i]){
            printf("Do both nasm and c implementations produce the same values?\n"" NO!  The LFSR differ!  index %i nasm is %i c is %i", i, nasmPseudoRandom[i], cPseudoRandom[i] );
            areBothEqual = 1;
            break;
        } 
    }

    if (areBothEqual == 0)
        printf(" Do both nasm and c implementations produce the same values?\n"" YES! The implementations for c and nasm gave the same resulting array. \n");

    double chiSquare = chiUniformExpected(cPseudoRandom);
    printf("chi_square %f \n", chiSquare);

    if(chiSquare > CRITICAL_VALUE)
        printf(" REJECTED hypothesis, chi-square: %f > %f (critical value)\n"" Numbers are NOT uniformly distributed. chi square > critical value for alpha = 0.05 and df 15\n"" (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    else
        printf(" ACEPTED hypothesis, chi-square: %f <= %f (critical value)\n"" Numbers are uniformly distributed. chi square <= critical value for alpha = 0.05 and df 15\n"" (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    
    printf("...Finished Chi Test\n\n");

   
    printf("time taken by nasm: %ld clocks, %ld miliseconds \n", clockNasm, clockNasm*1000/CLOCKS_PER_SEC);
    printf("time taken by c: %ld clocks, %ld miliseconds \n", clockC, clockC*1000/CLOCKS_PER_SEC);
    printf("c/nasm (clocks): %f  \n", (double)clockC/(double)clockNasm);

// extra report functions 

// print generated numbers
    // printf("Printing Pseudo randoms generated with nasm function... \n");
    //  for (int i = 0; i < ARRAY_SIZE; i++)
    //  {
    //     printf(" %i ", nasmPseudoRandom[i]);
    //  }
    // printf("Printed Pseudo randoms generated with nasm function. \n");

    // printf("Printing Pseudo randoms generated with c function... \n");
    //  for (int i = 0; i < ARRAY_SIZE; i++)
    //  {
    //     printf(" %i ", cPseudoRandom[i]);
    //  }
    // printf("Printed Pseudo randoms generated with c function. \n");

// // check if any nu,bers were repeated
    // for (int i = 0; i < ARRAY_SIZE; i++)
    // {
    //     for (int j = 0; j < ARRAY_SIZE; j++)
    //     {
    //         if( i != j && nasmPseudoRandom[i] == nasmPseudoRandom[j]){
    //              printf("Repeated number %i found in nasmPseudoRandom at indexes %i,  and %i", nasmPseudoRandom[j], i, j);
    //              hasRepeated = 1;
    //         }
    //     }
        
    // }
    // if (hasRepeated == 0){
    //     printf("No repeated numbers found in nasmPseudoRandom. \n");
    // }
    

    return 0;
}