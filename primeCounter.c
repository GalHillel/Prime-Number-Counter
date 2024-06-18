#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>

#define NUM_THREADS 8
#define BUFFER_SIZE 256

typedef struct
{
    long long buffer[BUFFER_SIZE];
    int start;
    int end;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
} buffer_t;

buffer_t buffer = {
    .start = 0,
    .end = 0,
    .count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond_nonempty = PTHREAD_COND_INITIALIZER,
    .cond_nonfull = PTHREAD_COND_INITIALIZER};

volatile int total_primes = 0;

long long mulmod(long long a, long long b, long long mod)
{
    long long x = 0, y = a % mod;
    while (b > 0)
    {
        if (b % 2 == 1)
        {
            x = (x + y) % mod;
        }
        y = (y * 2) % mod;
        b /= 2;
    }
    return x % mod;
}

long long modulo(long long base, long long exponent, long long mod)
{
    long long x = 1;
    long long y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            x = (x * y) % mod;
        }
        y = (y * y) % mod;
        exponent = exponent / 2;
    }
    return x % mod;
}

int Miller(long long p, int iteration)
{
    if (p < 2)
    {
        return 0;
    }
    if (p != 2 && p % 2 == 0)
    {
        return 0;
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
            return 0;
        }
    }
    return 1;
}

void *worker(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&buffer.mutex);
        while (buffer.count == 0)
        {
            pthread_cond_wait(&buffer.cond_nonempty, &buffer.mutex);
        }
        long long num = buffer.buffer[buffer.start];
        buffer.start = (buffer.start + 1) % BUFFER_SIZE;
        buffer.count--;
        pthread_cond_signal(&buffer.cond_nonfull);
        pthread_mutex_unlock(&buffer.mutex);

        if (num == -1)
            break;

        if (Miller(num, 5))
        {
            __sync_fetch_and_add(&total_primes, 1);
        }
    }
    return NULL;
}

void print_memory_usage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory usage: %ld KB\n", usage.ru_maxrss);
}

int main()
{
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    long long num;
    while (scanf("%lld", &num) != EOF)
    {
        pthread_mutex_lock(&buffer.mutex);
        while (buffer.count == BUFFER_SIZE)
        {
            pthread_cond_wait(&buffer.cond_nonfull, &buffer.mutex);
        }
        buffer.buffer[buffer.end] = num;
        buffer.end = (buffer.end + 1) % BUFFER_SIZE;
        buffer.count++;
        pthread_cond_signal(&buffer.cond_nonempty);
        pthread_mutex_unlock(&buffer.mutex);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_mutex_lock(&buffer.mutex);
        while (buffer.count == BUFFER_SIZE)
        {
            pthread_cond_wait(&buffer.cond_nonfull, &buffer.mutex);
        }
        buffer.buffer[buffer.end] = -1;
        buffer.end = (buffer.end + 1) % BUFFER_SIZE;
        buffer.count++;
        pthread_cond_signal(&buffer.cond_nonempty);
        pthread_mutex_unlock(&buffer.mutex);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("%d total primes.\n", total_primes);
    print_memory_usage();

    return 0;
}
