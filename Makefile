.PHONY: all
all: generator primeCounter

generator: generator.c
	gcc -o randomGenerator generator.c -O3

primeCounter: primeCounter.c
	gcc -o primeCounter primeCounter.c -O3 -lpthread

.PHONY: clean
clean:
	-rm randomGenerator primeCounter 2>/dev/null
