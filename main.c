// to run
// nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
// gcc -g -m32 main.c fib_lfsr.o -o lfsr
// ./lfsr 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    #define STARTING_BIT 0x800000// only the last 24 bits are 
    #define LAST_BIT 0x000001//
    #define BITS_24 0xFFFFFF// only the last 24 bits are 1
    #define BIT_MASK_TAPS 0x00001B // mask where only TAPS are one (24,22,21,20), big endian for cases like seed = 1
    #define ARRAY_SIZE 16777216 // 16777215 is the biggest 24bit int
    #define N_CLASSES 16
    int const CLASS_SIZE = ARRAY_SIZE/N_CLASSES;
    double const UNIFORM_FREQ =  (double)1/CLASS_SIZE; // the frequency expected for an ocurrence in a class is 1/size of class, in uniform distribution 
    double const ALMOST_ZERO_DENOMINATOR = 0.1;
    double const CRITICAL_VALUE = 7.879; // for alpha = 0.05 and df = 1
    #define SEED 1

int get_polynomial_a(int lfsr){
    int bit, aux;

    bit = (lfsr & BIT_MASK_TAPS);
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & LAST_BIT;
    bit = (bit << 23); // todo checl if this ever above 24

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

#ifdef _WIN32
int* lfsr(int);
#endif
#ifdef linux
int* _lfsr(int);
#endif

// null hypothesis: it is not random, numbers [0, 1M) are expected to be in 1st class
// df = 1. df = k - 1. k = 2. k is amount of categories: uniform and not uniform.
double chiUniformExpected(int* pseudoRandom);
double chiUniformExpected(int* pseudoRandom){
    printf("\nInitiating chi square test... \n");
    double chi_sqr = 0;  
    int pseudo = -1;
    double expFreq = (double)1/(double)ARRAY_SIZE; // all numbers in class have equal chance. double on numerator is enough but just in case
    double obsFreq;
    int classLowest = 0;
    int classHighest = 0;
    int pseudoOcurr = 0;

// counts to print frequencies as output txt file is too big. double for frequency calculation later
    double expNotObs = 0;
    double expOverObs = 0;
    double expObs = 0;
    double notExpObs = 0;
    double countOther = 0;

// allocating classes of pseudo randoms and their ocurrences
// classes[classIdx][idx], has pseudo and ocurrence. classes->class->index->pseudo|ocurrence.
    int*** classes = (int***)malloc(N_CLASSES * sizeof(int**)); // allocate space for each class (16 classes)
    for (int classIdx = 0; classIdx < N_CLASSES; classIdx++)
    {
        classes[classIdx] = (int**)malloc(ARRAY_SIZE/N_CLASSES * sizeof(int*)); // allocate space for pointer to each pseudo (1M pseudo per class)
        for (int pseudoIdx = 0; pseudoIdx < ARRAY_SIZE/N_CLASSES; pseudoIdx++)
        {
            classes[classIdx][pseudoIdx] = (int*)malloc(2 * sizeof(int)); // allocate space for value[0] and ocurrence[1] of each pseudo 
        }
        
    }

// populating classes with pseudorandoms and their ocurrences
// separate all pseudos in N_CLASSES
    for (int MClass = 0; MClass < N_CLASSES; MClass++) // 16 classes
    {
        for (int index = 0, i = 0; i < ARRAY_SIZE/N_CLASSES; i++) // iterate through each class without separating pseudos 
        {
            index = i;
            classes[MClass][index][0] = pseudoRandom[index]; // put pseudo in class
            classes[MClass][index][1] += 1; // increase counter of ocurrence of pseudo in this class. malloc ensures it starts at 0
        }
        
    }



// get chi_sqr and counts
    for (int classIdx = 0; classIdx < N_CLASSES; classIdx++) // 16 classes
    {
        classLowest = classIdx * ARRAY_SIZE/N_CLASSES + 1;
        classHighest = (classIdx + 1) * (ARRAY_SIZE)/N_CLASSES;
        expNotObs = 0;     expOverObs = 0;     expObs = 0;     notExpObs = 0;   countOther = 0;
        printf(" class %i (%i...%i]:\n", classIdx, classLowest, classHighest);

        for (int index = 0, i = 0; index < ARRAY_SIZE/N_CLASSES; index++) // iterate through each class without separating pseudos 
        {
            // a number is expected to be in a class of uniform distribution when
            // it's within the bounds of the class, and exp freq is UNIFORM_FREQ;
            // otherwise exp freq is 0
            pseudo = classes[classIdx][index][0]; // value of pseudo
            pseudoOcurr = (double)1/classes[classIdx][index][1]; // ocurrences of pseudo in class 


            switch (pseudoOcurr)
            {
            case 0:
                expNotObs++;
                break;
            case 1:
                expObs++;
                break;
            default:
                countOther++;
                break;
            }

            obsFreq = (double)pseudoOcurr/CLASS_SIZE;

            if(pseudo < classLowest || pseudo > classHighest){
                expFreq = ALMOST_ZERO_DENOMINATOR; // cant have 0 denominator
                notExpObs++; // number was not expected in this class but was observed
            }
            else{
                expFreq = UNIFORM_FREQ;
                expObs++; // number was expected and observed within group
            }
            
            // printf("[%i]; pseudo random: %i; obsFreq: %f; expFreq: %f; \n", classIdx, pseudo, obsFreq, expFreq);
            chi_sqr += (obsFreq - expFreq) * (obsFreq - expFreq) / (expFreq);
        }
        printf(
    "  Expected and Observed: %.0f (%f) ;\n"
    "  Expected and Not Observed: %.0f (%f);\n"
    "  Not Expected and Observed: %.0f (%f);\n"
    "  Other: %.0f (%f);\n\n",
    expObs, expObs / ARRAY_SIZE,
    expNotObs, expNotObs / ARRAY_SIZE,
    notExpObs, notExpObs / ARRAY_SIZE,
    countOther, countOther / ARRAY_SIZE
);
    }
    return chi_sqr;
}


int main(){
    int* nasmPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    int* cPseudoRandom = (int*)malloc(ARRAY_SIZE * sizeof(int));
    double chi_sqr = 0;
    clock_t clockNasm, clockC ;  // function execution takes less than a second, must use clock in ANSI C. https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c.
    double chiSquare = 0;
    int areBothEqual = 0;
    int hasRepeated = 0;
    
    clockNasm = clock();
    printf("Calling LFSR in nasm, SEED: %i ...", SEED);
    #ifdef _WIN32
    nasmPseudoRandom = lfsr(SEED);
    #endif
    #ifdef linux
    nasmPseudoRandom = _lfsr(SEED);
    #endif

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
            printf("Do both nasm and c implementations produce the same values? NO!  The LFSR differ!  index %i nasm is %i c is %i", i, nasmPseudoRandom[i], cPseudoRandom[i] );
            areBothEqual = 1;
            break;
        } 
    }

    if (areBothEqual == 0)
        printf(" Do both nasm and c implementations produce the same values? YES! The implementations for c and nasm gave the same resulting array. \n");

    chiSquare = chiUniformExpected(cPseudoRandom);
    printf("chi_square %f \n", chiSquare);

    if(chiSquare > CRITICAL_VALUE)
        printf(" REJECTED hypothesis, chi-square: %f > %f (critical value)\n"" Numbers are NOT uniformly distributed.chi square > critical value for alpha = 0.05 and df 1\n"" (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    else
        printf(" ACEPTED hypothesis, chi-square: %f <= %f (critical value)\n"" Numbers are uniformly distributed.chi square <= critical value for alpha = 0.05 and df 1\n"" (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    
    printf("...Finished Chi Test\n\n");

   
    printf("time taken by nasm: %ld clocks, %ld secs \n", clockNasm, clockNasm/CLOCKS_PER_SEC);
    printf("time taken by c: %ld clocks, %ld secs \n", clockC, clockC/CLOCKS_PER_SEC);
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