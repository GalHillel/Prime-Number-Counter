#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 8
#define BUFFER_SIZE 4096
#define RAM_LIMIT (2 * 1024 * 1024) // 2MB

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;

int buffer[BUFFER_SIZE];
int buffer_count = 0;
int total_counter = 0;
bool done = false;

long long modulo(long long base, long long exponent, long long mod)
{
    long long result = 1;
    base %= mod;
    while (exponent)
    {
        if (exponent % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exponent /= 2;
    }
    return result;
}

long long mulmod(long long a, long long b, long long mod)
{
    long long result = 0;
    a %= mod;
    while (b)
    {
        if (b % 2 == 1)
            result = (result + a) % mod;
        a = (a * 2) % mod;
        b /= 2;
    }
    return result;
}

bool is_small_prime(long long p)
{
    const int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    for (int i = 0; i < sizeof(primes) / sizeof(primes[0]); i++)
    {
        if (p == primes[i])
            return true;
        if (p % primes[i] == 0)
            return false;
    }
    return true;
}

bool MillerRabin(long long p, int iteration)
{
    if (p < 2)
    {
        return false;
    }
    if (is_small_prime(p))
    {
        return true;
    }
    if (p != 2 && p % 2 == 0)
    {
        return false;
    }
    long long s = p - 1;
    while (s % 2 == 0)
    {
        s /= 2;
    }
    for (int i = 0; i < iteration; i++)
    {
        long long a = rand() % (p - 1) + 1, temp = s;
        long long mod = modulo(a, temp, p);
        while (temp != p - 1 && mod != 1 && mod != p - 1)
        {
            mod = mulmod(mod, mod, p);
            temp *= 2;
        }
        if (mod != p - 1 && temp % 2 == 0)
        {
            return false;
        }
    }
    return true;
}

void *prime_counter_thread(void *arg)
{
    int iteration = 5; // Number of iterations for Miller-Rabin
    while (1)
    {
        pthread_mutex_lock(&buffer_mutex);
        while (buffer_count == 0 && !done)
        {
            pthread_cond_wait(&buffer_not_empty, &buffer_mutex);
        }
        if (buffer_count == 0 && done)
        {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }

        int num = buffer[--buffer_count];
        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&buffer_mutex);

        if (MillerRabin(num, iteration))
        {
            __sync_fetch_and_add(&total_counter, 1);
        }
    }
    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_create(&threads[i], NULL, prime_counter_thread, NULL);
    }

    int num;
    while (scanf("%d", &num) != EOF)
    {
        pthread_mutex_lock(&buffer_mutex);
        while (buffer_count == BUFFER_SIZE)
        {
            pthread_cond_wait(&buffer_not_full, &buffer_mutex);
        }

        buffer[buffer_count++] = num;
        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&buffer_mutex);
    }

    pthread_mutex_lock(&buffer_mutex);
    done = true;
    pthread_cond_broadcast(&buffer_not_empty);
    pthread_mutex_unlock(&buffer_mutex);

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    printf("%d total primes.\n", total_counter);

    return 0;
}
