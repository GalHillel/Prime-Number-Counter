#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <seed> <count>\n", argv[0]);
        return 1;
    }

    // Extract seed and count from command line arguments
    int seed = atoi(argv[1]);
    int count = atoi(argv[2]);

    // Set seed for random number generator
    srand(seed);
    
    int lowerLimit = 1000000;
    int upperLimit = 2100000000;

    // Start measuring time
    clock_t start_time = clock();

    // Generate and output random numbers
    for (int i = 0; i < count; ++i) {
        int random_number = rand() % (upperLimit - lowerLimit + 1) + lowerLimit;
        printf("%d\n", random_number);
        fflush(stdout);
    }

    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    // printf("Execution time : %.6f seconds\n", elapsed_time);

    return 0;
}
