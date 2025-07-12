/*

Name        : Netwig, Chad
Project     : Fuzz Them All
Date        : April 25, 2023
File Name   : fuzzer.c

Description : fuzzer takes in two command line arguments <prng_seed> <num_iterations>, which
            : are both 32-bit unsigned integers, and uses these values to output chars (bytes)
            : to stdout. This output consists of char values from 0 to 255 that are randomized
            : and mutated to produce an output to be piped in as stdin "fuzzed" input into
            : a target program. An example of the command line usage is:
            :
            :           ./fuzzer <prng_seed> <num_iterations> | ./<target_prog>
            :
            : The fuzzer will look in its current directory for a file called "seed" which
            : is a binary file of variable bytes that contains chars that will be used
            : for the intial_seed input array of chars. If no seed file is found, fuzzer will 
            : use the DEFAULT_SEED macro as the intial_seed input. In both cases, memory
            : is allocated with the exact amount of bytes necessary to hold this array.
            :
            : The main loop will iterate <num_iterations> times and will mutate (change)
            : each byte in the input_seed array with a probability of PROBABILITY macro.
            : If the iteration reaches EXTEND_INPUT_IT, an addtional 10 random bytes will
            : be added to the input_seed array and adjusted in memory to hold them, accordingly.
            :
            : The rand() function is seeded by the <prng_seed> value passed in from the
            : command line and is used for all pseudo-random number generation throughout
            : the program.
            :
            : After all iterations, fuzzer will print each element (char) of the input_seed
            : array to stdout (fuzzed output)
            
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Setup default seed as a macro
#define DEFAULT_SEED ((unsigned char[]) {16, 0, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 10})
#define PROBABILITY 13
#define EXTEND_INPUT_IT 500

/* prototypes */
int read_seed(unsigned char **input_seed);
void fuzz_out(unsigned char *input_seed, int num_bytes);
void mutate_bytes(unsigned char *input_seed, int num_bytes, unsigned int prng_seed);

/* fuzzer loop */
int main(int argc, char *argv[]) {
    unsigned char *input_seed;
    unsigned int prng_seed, num_iterations;
    int num_bytes = 0;

    if (argc != 3) {
        printf("Usage: %s <prng_seed> <num_iterations>\n", argv[0]);
        return -1;
    } else {
        prng_seed = (unsigned int)strtoul(argv[1], NULL, 0);
        num_iterations = (unsigned int)strtoul(argv[2], NULL, 0);
    }

    // debug
    // prng_seed = 1;
    // num_iterations = 10;

    // reads in seed file and populates input_seed char array. If no seed file is found
    // uses DEFAULT_SEED macro for char array
    num_bytes = read_seed(&input_seed);

    // main loop
    for (int i = 0; i < num_iterations; i++){

        if ((i+1) % EXTEND_INPUT_IT == 0) {  // check if 500 iterations have passed
            srand(prng_seed);
            input_seed = realloc(input_seed, num_bytes + 10);  // extend the array on heap by 10 bytes
           
            for (int j = 0; j < 10; j++) {  // add 10 random characters to the end of the array
                input_seed[num_bytes + j] = (unsigned char)(rand() % 256);
            }

            num_bytes += 10;  // update the number of bytes in the array
        }

        mutate_bytes(input_seed, num_bytes, prng_seed); // mutate bytes based on PROBABILITY macro
        // fuzz_out(input_seed, num_bytes); // send fuzzed chars to stdout
        //++ prng_seed;
    }

    // debug output
    // printf("\nPRNG seed: %u\n", prng_seed);
    // printf("Number of iterations: %u\n", num_iterations);

    fuzz_out(input_seed, num_bytes); // send fuzzed chars to stdout
    // free memory from heap
    free(input_seed);
    return 0;
}

/* read_seed function reads a filed called 'seed' in the current directory and places each
   byte into the input_seed char array. If the file is not found, it will place all the
   bytes from the DEFAULT_SEED macro into the input_seed char array. In either case, the
   populated input_seed char array will be used for the intial starting point for fuzzed output */
int read_seed(unsigned char **input_seed) {
    FILE *fp;
    int n;

    fp = fopen("seed", "rb");
    if (fp == NULL) {
        // perror("Error opening file");
        // no seed file, so establishes memory on heap for input_seed
        // char array to hold DEFAULT_SEED and copies it into the input_seed array
        *input_seed = (unsigned char *)malloc(sizeof(DEFAULT_SEED));
        memcpy(*input_seed, DEFAULT_SEED, sizeof(DEFAULT_SEED));
        n = sizeof(DEFAULT_SEED);
    } else {
        fseek(fp, 0L, SEEK_END);
        n = ftell(fp);
        rewind(fp);
        // allocates memory on heap to the bytes size in seed file
        *input_seed = (unsigned char *)malloc(n);
        if (*input_seed == NULL) {
            perror("Error allocating memory");
            return -1;
        }
        // reads bytes from file and places them in input_seed array on heap
        n = fread(*input_seed, sizeof(unsigned char), n, fp);
        if (n == 0) {
            perror("Error reading file");
            return -1;
        }
        fclose(fp);
    }

    return n;
}

/* fuzz_out iterates the input_seed array and sends to stdout for fuzz */
void fuzz_out(unsigned char *input_seed, int num_bytes) {
    //printf("%.*s", num_bytes, input_seed);
    for (int i = 0; i < num_bytes; i++) {
        printf("%c", input_seed[i]);
    }
}

/* mutate_bytes function randomly changes each char in the input_seed
   array based on a 13% probabilty (PROBABILITY macro) */
void mutate_bytes(unsigned char *input_seed, int num_bytes, unsigned int prng_seed) {
    unsigned int probability = 0;
    srand(prng_seed);
    for (int i = 0; i < num_bytes; i++) {
        probability = (rand() %100);
        if (probability <= PROBABILITY) {
            input_seed[i] = (unsigned char)(rand() % 256);
        }
    }
}
