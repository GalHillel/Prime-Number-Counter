
# Prime Number Counter

## Description
This project is designed to count prime numbers from an endless data stream, using multi-threading to optimize performance and ensure all CPU cores are utilized. The program is limited to using only 1MB of RAM.

## Files
- `generator.c`: Generates a stream of random numbers.
- `primeCounter.c`: Counts prime numbers from the generated stream using a multi-threaded approach.

## Requirements
- GCC Compiler

## Compilation
To compile the programs, use the following commands:
```bash
make all
```

## Usage
Generate a stream of random numbers and count the primes:
```bash
./generator <seed> <num_of_numbers> | ./primeCounter
```
Example:
```bash
./generator 10 10000000 | ./primeCounter
```

## Performance Measurement
To measure the performance, use the `time` command:
```bash
time ./generator 10 10000000 | ./primeCounter
```

## Notes
- The program uses a lock-free queue to minimize the overhead of thread synchronization.
- The `isPrime` function is optimized to handle larger numbers more efficiently.
- The solution ensures that RAM usage is below 1MB by carefully managing the queue size and avoiding unnecessary memory allocations.

## File Descriptions

### generator.c
This file generates a stream of random numbers based on a provided seed and count. The numbers range between 1,000,000 and 2,100,000,000. It uses the `rand()` function for number generation and outputs each number to the standard output.

### main.c
This file contains the main logic for the prime counting program. It includes:
- Initialization of a queue structure to hold numbers.
- The `isPrime` function to check for prime numbers.
- The `primeCounter` function which each thread runs to process numbers from the queue.
- The `enqueue` function to add numbers to the queue.
- The main function to create threads, read numbers, and manage the queue.
