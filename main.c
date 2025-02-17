// to run
// nasm -g -f elf32 lfsr.asm 
// gcc -g -m32 main.c lfsr.o -o lfsr
// ./lfsr 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    #define SEED 1
    #define LAST_BIT 0x000001//
    #define BITS_24 0xFFFFFF// only the last 24 bits are 1
    #define BIT_MASK_TAPS 0x00001B // mask where only TAPS are one (24,22,21,20). TAPS must be even amount and setwise co-prime .
    #define ARRAY_SIZE 16777216 // number of numbers to generate (16777216 is maximum supported)
    #define N_CLASSES 16
    int const CLASS_SIZE = (BITS_24+1)/N_CLASSES;
    // get the critical value in this table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf
    double const CRITICAL_VALUE = 25.00; // for alpha = 0.05 and df = N_classes -1 = 15

int get_polynomial_a(int lfsr){
    int bit;

    bit = (lfsr & BIT_MASK_TAPS);
 // bit =  x^24 + x^23        + x^21        + x^20         + 1
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & LAST_BIT;
    bit = (bit << 23);

    return bit;
}


int* lfsr_array(int* pseudoRandom, int lfsr);
int* lfsr_array(int* pseudoRandom, int lfsr){
    int polynomial;

    for (int i = 0; i < ARRAY_SIZE; i++){
        polynomial = get_polynomial_a(lfsr);
        lfsr = (lfsr >> 1) | polynomial;

        pseudoRandom[i] = lfsr;
    }
    
    return pseudoRandom;
 
}

#ifdef _WIN32
    extern int* lfsr(int lfsr, int n);
#else
    extern int* _lfsr(int lfsr, int n);
#endif


// null hypothesis: it is uniform distribution
double chiUniformExpected(int* pseudoRandom) {
    printf("\nInitiating chi square test... \n");
    printf("\tThe hypoteshis: generated distribution is uniform\n");
    double chi = 0;
    const double expectedFrequency = (double)ARRAY_SIZE/(double)N_CLASSES; // if array_size = 2^24, expectedFrequency = 1.048.576

    int* observationsPerClass = calloc(N_CLASSES,sizeof(int)); // 0 initialize at calloc

    for (int i = 0; i < ARRAY_SIZE; i++) // iterate through each pseudorandom number and associating it to a class
    {
        observationsPerClass[pseudoRandom[i]/CLASS_SIZE]++;
    }


    printf("\t%15s %12s %8s  %8s\n", "Number in class", "", "Observed", "Expected");
    // Calculating chi
    for (int n = 0; n < N_CLASSES; n++) // 16 classes
    {
        printf("\tClass (%8d, %8d] : %7d | %7.2f\n", n*CLASS_SIZE, (n+1)*CLASS_SIZE, observationsPerClass[n], expectedFrequency);
        double a = (observationsPerClass[n] - expectedFrequency);
        chi += a*a;
    }
    chi /= expectedFrequency;

    free(observationsPerClass);
    return chi;
}


int main(){
    if(SEED == 0){
        printf("Seed is 0, pick another by altering `#DEFINE SEED` line on top of main.c . \n\tThis would cause every following number generated to be 0,as 0 xor 0 = 0\n");
        return 0;
    }

    int* nasmPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    int* cPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    clock_t clockNasm, clockC ; 
    int areBothEqual = 0;
    int hasRepeated = 0;

    printf("Number of pseudo randoms to generate: %d\n", ARRAY_SIZE);

    printf("Calling LFSR in nasm, SEED: %i ...", SEED);
    clockNasm = clock();
    #ifdef _WIN32
    nasmPseudoRandom = lfsr(SEED, ARRAY_SIZE);
    #endif
    #ifdef linux
    nasmPseudoRandom = _lfsr(SEED, ARRAY_SIZE);
    #endif
    clockNasm = clock() - clockNasm;
    printf("Finished\n");

    printf("Calling LFSR in c, SEED: %i ...", SEED);
    clockC = clock();
    cPseudoRandom = lfsr_array(cPseudoRandom, SEED);
    clockC = clock() - clockC;
    printf("Finished\n");
    printf("\n");

// check if both implementations produced the same values in the same order
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        if(nasmPseudoRandom[i] != cPseudoRandom[i]){
            printf("Do both nasm and c implementations produce the same values?\n""\tNO!  The LFSR differ!  index %i nasm is %i c is %i", i, nasmPseudoRandom[i], cPseudoRandom[i] );
            areBothEqual = 1;
            break;
        } 
    }

    if (areBothEqual == 0)
        printf("Do both nasm and c implementations produce the same values?\n""\tYES! The implementations for c and nasm gave the same resulting array. \n");

    double chiSquare = chiUniformExpected(cPseudoRandom);
    printf("\tchi_square %f \n", chiSquare);

    if(chiSquare > CRITICAL_VALUE)
        printf("\tREJECTED hypothesis, chi-square: %f > %f (critical value)\n""\tNumbers are NOT uniformly distributed. chi square > critical value for alpha = 0.05 and df 15\n"" (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    else
        printf("\tNot found enough evidence to reject the hypothesis, chi-square: %f <= %f (critical value)\n""\tNumbers are uniformly distributed. chi square <= critical value for alpha = 0.05 and df 15\n""\t(chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    
    printf("...Finished Chi Test\n\n");

   
    printf("time taken by nasm: %ld clocks, %ld miliseconds \n", clockNasm, clockNasm*1000/CLOCKS_PER_SEC);
    printf("time taken by c: %ld clocks, %ld miliseconds \n", clockC, clockC*1000/CLOCKS_PER_SEC);
    printf("c/nasm (clocks): %f  \n\n", (double)clockC/(double)clockNasm);


    unsigned short* aparicoes = calloc(BITS_24+1,sizeof(short)); // initialize 0 array

    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        aparicoes[nasmPseudoRandom[i]] += 1;
    }

    printf("Check to see if all the numbers were generated...\n");
    if (ARRAY_SIZE >= BITS_24) {
        for (size_t i = 0; i < BITS_24+1; i++)
        {
            if (aparicoes[i] > 1) {
                printf("\tError, the number %d was generated %d times\n", i, aparicoes[i]);
            }
            if (aparicoes[i] == 0) {
                if (i != 0) {
                    printf("\tError, the number %d was generated 0 times\n", i);
                } else {
                    printf("\tZero was generated 0 times, as espected\n");
                }
            }
        }
    } else {
        printf("There is no reason to check if not all possible numbers got a chance to be generated (ARRAY_SIZE too small)\n");
    }
    printf("\n");
    
    

// extra report functions 
    printf("5 first pseudorandoms generated.\n");
     for (int i = 0; i < 5  ; i++)
     {
        printf("\t%i \n", nasmPseudoRandom[i]);
     }
    printf("5 last pseudorandoms generated\n");
     for (int i = ARRAY_SIZE - 5; i < ARRAY_SIZE ; i++)
     {
        printf("\t%i \n", nasmPseudoRandom[i]);
     }

    // printf("Printing all Pseudo randoms generated with c function... \n");
    //  for (int i = 0; i < ARRAY_SIZE; i++)
    //  {
    //     printf(" %i ", cPseudoRandom[i]);
    //  }
    // printf("Printed Pseudo randoms generated with c function. \n");

    printf("\nIs LFSR sequence maximal-length? \n");
// // check if any numbers were repeated
    for (int i = ARRAY_SIZE -1; i < ARRAY_SIZE; i++)
    {
        for (int j = 0; j < ARRAY_SIZE; j++)
        {
            if( i != j && nasmPseudoRandom[i] == nasmPseudoRandom[j]){
                //  printf(" Repeated number %i found in nasmPseudoRandom at indexes %i,  and %i\n", nasmPseudoRandom[j], i, j);
                 hasRepeated = 1;

            }
        }
        
    }

    switch(hasRepeated)
    {
        case 0:
            printf("\tI don't Know: Seed was not repeated. IS ARRAY_SIZE set to under 2^24? \n");
            break;
        case 1:
            printf("\tYES! Seed was only repeated once\n");
            break;
        default:
            printf("\tNO! Seed was repeated over once \n");
            break;
    }
    

    return 0;
}