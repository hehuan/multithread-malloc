CC = gcc
CFLAGS = -O2 -Wall
INCDIR_JEMALLOC = -I /usr/local/include/jemalloc/


malloc: multithread-malloc-benchmark
jemalloc: multithread-jemalloc-benchmark
all: malloc jemalloc

multithread-malloc-benchmark: multithread-malloc-benchmark.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread 

multithread-jemalloc-benchmark: multithread-malloc-benchmark.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread $(INCDIR_JEMALLOC) -ljemalloc

.PHONY: clean
clean:
	rm -f multithread-malloc-benchmark multithread-jemalloc-benchmark
