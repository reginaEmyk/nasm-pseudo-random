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
        printf("bit is bigger than array size");
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
    double chi_sqr = 0;  
    double* chi_sqr_classes = (double*)malloc(n_classes * sizeof(double)); 
    int pseudo = -1;
    double expected_freq = (double)1/(double)array_size; // all numbers in class have equal chance. double on numerator is enough but just in case
    double obs_freq;
    int class_lowest = 0; // todo uniform typing
    int class_highest = 0;
    int pseudoOcurr = 0;

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
            // printf(" %i", index);
        }
        
    }



// get chi_sqr
    for (int classIdx = 0; classIdx < n_classes; classIdx++) // 16 classes
    {
        class_lowest = classIdx * array_size/n_classes;
        class_highest = classIdx * (array_size + 1)/n_classes - 1;
        for (int index = 0, i = 0; i < array_size/n_classes; i++) // iterate through each class without separating pseudos 
        {
            // a number is expected to be in a class of uniform distribution when
            // it's within the bounds of the class, and exp freq is UNIFORM_FREQ;
            // otherwise exp freq is 0
            pseudo = classes[classIdx][index][0];
            pseudoOcurr = (double)1/classes[classIdx][index][1];

            if(pseudoOcurr == 0)
                pseudoOcurr = ALMOST_ZERO_DENOMINATOR;

            obs_freq = (double)pseudoOcurr/CLASS_SIZE;

            if(pseudo < class_lowest || pseudo > class_highest) 
                expected_freq = ALMOST_ZERO_DENOMINATOR; // cant have 0 denominator
            else
                expected_freq = UNIFORM_FREQ;


            chi_sqr += (obs_freq - expected_freq) * (obs_freq - expected_freq) / (expected_freq);
        }
        
    }

    return chi_sqr;
}



int main(){
    // printf("Hello, World!");
    // 16777216
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

    printf("calling 16777215 \n;");
    clock_nasm = clock();
    nasmPseudoRandom = _lfsr(SEED);
    clock_nasm = clock() - clock_nasm;

    clock_c = clock();
    cPseudoRandom = lfsr_array(cPseudoRandom, SEED);
    clock_c = clock() - clock_c;
// todo fix:   WRONG  index 16777215 nasm is 8388608 c is 0
// todo test for both arrays
    chiSquare = chiUniformExpected(cPseudoRandom);
    printf("chi_square %f ", chiSquare);
    if(chiSquare > CRITICAL_VALUE){
        printf(" REJECTED hypothesis, %f > %f . Numbers are NOT uniformly distributed.chi square > critical value for alpha = 0.05 and df 1. (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n ", chiSquare, CRITICAL_VALUE);
    } else{
        printf(" ACEPTED hypothesis, %f <= %f . Numbers are uniformly distributed.chi square <= critical value for alpha = 0.05 and df 1. (chi dist table https://people.smp.uq.edu.au/YoniNazarathy/stat_models_B_course_spring_07/distributions/chisqtab.pdf)\n", chiSquare, CRITICAL_VALUE);
    }

    for (int i = 0; i < array_size; i++)
    {
        if(nasmPseudoRandom[i] != cPseudoRandom[i]){
            printf(" WRONG  index %i nasm is %i c is %i", i, nasmPseudoRandom[i], cPseudoRandom[i] );
            areBothEqual = 1;
            break;
        } 
    }

    if (areBothEqual == 0)
        printf("The implementations for c and nasm gave the same resulting array. \n");

    
// todo print arrays
// todo determine acceptance rejection regions?


    // for (int i = 0; i < array_size; i++)
    // {
    //     printf(" %i; ", pseudoRandom[i]);
    // }


// todo chi's region of acceptance/rejection?


    
    printf("\n time taken by nasm: %ld clocks, %ld secs", clock_nasm, clock_nasm/CLOCKS_PER_SEC);
    printf("\n time taken by c: %ld clocks, %ld secs \n", clock_c, clock_c/CLOCKS_PER_SEC);
    printf("\n c/nasm (clocks): %f  \n", (double)clock_c/(double)clock_nasm);



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