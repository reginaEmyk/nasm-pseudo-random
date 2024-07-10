#include <stdio.h>
#include <stdlib.h>
#include <time.h>
    #define _STARTING_BIT_ 0x800000// only the last 24 bits are 
    #define _LAST_BIT_ 0x000001//
    #define _24_BITS_ 0xFFFFFF// only the last 24 bits are 1
    #define _BIT_MASK_TAPS_ 0x00001B // mask where only TAPS are one (24,22,21,20), big endian for cases like seed = 1
    #define array_size 16777216 // 16777215 is the biggest 24bit int
    #define n_classes 16
    int const CLASS_SIZE = array_size/n_classes;
    double const UNIFORM_FREQ =  (double)1/CLASS_SIZE; // the frequency expected for an ocurrence in a class is 1/size of class, in uniform distribution 
    double const ALMOST_ZERO_DENOMINATOR = 0.1;
    double const CRITICAL_VALUE = 7.879; // for alpha = 0.05 and df = 1
    #define SEED 1

// alpha 

int get_polynomial(int lfsr){
    int bit, aux;

    bit = (lfsr & _BIT_MASK_TAPS_);
    bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 4)) & _LAST_BIT_;
    bit = (bit << 23); // todo checl if this ever above 24

    if (bit >= array_size){
        // printf("bit is bigger than array size");
        exit(1);
    }

    return bit;
}


int* lfsr_array(int* pseudoRandom, int lfsr);
int* lfsr_array(int* pseudoRandom, int lfsr){
    int polynomial = -1;

    for (int i = 0; i < array_size; i++){
        polynomial = get_polynomial(lfsr);
        lfsr = (lfsr >> 1) | polynomial;

        pseudoRandom[i] = lfsr;
    }
    
    return pseudoRandom;
 
}


int* _lfsr(__int32_t);

// null hypothesis: it is not random, numbers [0, 1M) are expected to be in 1st class
// df = 1. df = k - 1. k = 2. k is amount of categories: uniform and not uniform.
double chiUniformExpected(int* pseudoRandom);
double chiUniformExpected(int* pseudoRandom){
    // printf("Initiating chi square test... \n");
    double chi_sqr = 0;  
    double* chi_sqr_classes = (double*)malloc(n_classes * sizeof(double)); 
    int pseudo = -1;
    double expected_freq = (double)1/(double)array_size; // all numbers in class have equal chance. double on numerator is enough but just in case
    double obs_freq;
    int class_lowest = 0; // todo uniform typing
    int class_highest = 0;
    int pseudoOcurr = 0;

// counts to print frequencies as output txt file is too big. double for frequency calculation later
    double expNotObs = 0;
    double expOverObs = 0;
    double expObs = 0;
    double notExpObs = 0;
    double countOther = 0;

// allocating classes of pseudo randoms and their ocurrences
// classes[classIdx][idx], has pseudo and ocurrence. classes->class->index->pseudo|ocurrence.
    int*** classes = (int***)malloc(n_classes * sizeof(int**)); // allocate space for each class (16 classes)
    for (int classIdx = 0; classIdx < n_classes; classIdx++)
    {
        classes[classIdx] = (int**)malloc(array_size/n_classes * sizeof(int*)); // allocate space for pointer to each pseudo (1M pseudo per class)
        for (int pseudoIdx = 0; pseudoIdx < array_size/n_classes; pseudoIdx++)
        {
            classes[classIdx][pseudoIdx] = (int*)malloc(2 * sizeof(int)); // allocate space for value[0] and ocurrence[1] of each pseudo 
        }
        
    }

// populating classes with pseudorandoms and their ocurrences
// separate all pseudos in n_classes
    for (int MClass = 0; MClass < n_classes; MClass++) // 16 classes
    {
        for (int index = 0, i = 0; i < array_size/n_classes; i++) // iterate through each class without separating pseudos 
        {
            index = i;
            classes[MClass][index][0] = pseudoRandom[index]; // put pseudo in class
            classes[MClass][index][1] += 1; // increase counter of ocurrence of pseudo in this class. malloc ensures it starts at 0
        }
        
    }



// get chi_sqr and counts
    for (int classIdx = 0; classIdx < n_classes; classIdx++) // 16 classes
    {
        class_lowest = classIdx * array_size/n_classes;
        class_highest = (classIdx + 1) * (array_size)/n_classes - 1;
        expNotObs = 0;     expOverObs = 0;     expObs = 0;     notExpObs = 0;   countOther = 0;
        printf("class %i [%i...%i]: ", classIdx, class_lowest, class_highest);

        for (int index = 0, i = 0; index < array_size/n_classes; index++) // iterate through each class without separating pseudos 
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

            obs_freq = (double)pseudoOcurr/CLASS_SIZE;

            if(pseudo < class_lowest || pseudo > class_highest){
                expected_freq = ALMOST_ZERO_DENOMINATOR; // cant have 0 denominator
                notExpObs++; // number was not expected in this class but was observed
            }
            else{
                expected_freq = UNIFORM_FREQ;
                expObs++; // number was expected and observed within group
            }
            
            // printf("[%i]; pseudo random: %i; obs_freq: %f; expected_freq: %f; \n", classIdx, pseudo, obs_freq, expected_freq);
            chi_sqr += (obs_freq - expected_freq) * (obs_freq - expected_freq) / (expected_freq);
        }
        printf(" Expected and Observed: %.0f (%f), ; Expected and Not Observed: %.0f (%f); Not Expected and Observed: %.0f (%f); Other: %.0f (%f); \n \n", expObs, expObs/array_size, expNotObs, expNotObs/array_size, notExpObs, notExpObs/array_size, countOther);
        
    }
    // printf("Exiting chi square test... \n");

    return chi_sqr;
}


// void printFrequency(int* pseudoRandom, int* frequency);
// void printFrequency(int* pseudoRandom, int* frequency){
//     int pseudo = -1;
//     int pseudoOcurr = 0;
//     int class_lowest = 0; // todo uniform typing
//     int class_highest = 0;

//     for 
// }

int main(){
    int* nasmPseudoRandom = (int*)malloc(array_size * sizeof(int));
    int* cPseudoRandom = (int*)malloc(array_size * sizeof(int));
    int* pseudR_frequency = (int*)malloc(array_size * sizeof(int));
    int count_ocurr = 0;
    double freq_ob = 0; 
    double freq_exp = 1/array_size;
    double one_freq = freq_exp;
    double chi_sqr = 0;
    clock_t clock_nasm, clock_c ;  // function execution takes less than a second, must use clock in ANSI C. https://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c.
    double chiSquare = 0;
    int areBothEqual = 0;
    int hasRepeated = 0;

    
    clock_nasm = clock();
    printf("Calling LFSR in nasm, SEED: %i ...\n", SEED);
    nasmPseudoRandom = _lfsr(SEED);
    clock_nasm = clock() - clock_nasm;

    clock_c = clock();
    printf("Calling LFSR in c, SEED: %i ...\n", SEED);
    cPseudoRandom = lfsr_array(cPseudoRandom, SEED);
    clock_c = clock() - clock_c;

    for (int i = 0; i < array_size; i++)
    {
        if(nasmPseudoRandom[i] != cPseudoRandom[i]){
            printf(" Do both nasm and c implementations produce the same values? NO!  The LFSR differ!  index %i nasm is %i c is %i", i, nasmPseudoRandom[i], cPseudoRandom[i] );
            areBothEqual = 1;
            break;
        } 
    }

    if (areBothEqual == 0){
        printf(" Do both nasm and c implementations produce the same values? YES! The implementations for c and nasm gave the same resulting array. \n");
// since both are known to be equal and the output is huge, only reporting for one lfsr
        chiSquare = chiUniformExpected(cPseudoRandom);
        printf("chi_square %f \n", chiSquare);
        if(chiSquare > CRITICAL_VALUE){
            printf(" REJECTED hypothesis, chi-square: %f > %f (critical value). Numbers are NOT uniformly distributed.chi square > critical value for alpha = 0.05 and df 1. (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n ", chiSquare, CRITICAL_VALUE);
        } else{
            printf(" ACEPTED hypothesis, chi-square: %f <= %f (critical value). Numbers are uniformly distributed.chi square <= critical value for alpha = 0.05 and df 1. (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    }

}
   
    printf("\n time taken by nasm: %ld clocks, %ld secs", clock_nasm, clock_nasm/CLOCKS_PER_SEC);
    printf("\n time taken by c: %ld clocks, %ld secs \n", clock_c, clock_c/CLOCKS_PER_SEC);
    printf("\n c/nasm (clocks): %f  \n", (double)clock_c/(double)clock_nasm);

// extra report functions 

// print generated numbers
    // printf("Printing Pseudo randoms generated with nasm function... \n");
    //  for (int i = 0; i < array_size; i++)
    //  {
    //     printf(" %i ", nasmPseudoRandom[i]);
    //  }
    // printf("Printed Pseudo randoms generated with nasm function. \n");

    // printf("Printing Pseudo randoms generated with c function... \n");
    //  for (int i = 0; i < array_size; i++)
    //  {
    //     printf(" %i ", cPseudoRandom[i]);
    //  }
    // printf("Printed Pseudo randoms generated with c function. \n");

// // check if any nu,bers were repeated
    // for (int i = 0; i < array_size; i++)
    // {
    //     for (int j = 0; j < array_size; j++)
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