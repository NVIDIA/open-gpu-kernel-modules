CC ?= gcc
CFLAGS ?= -O2 -pthread -std=c11 -Wall -Wextra -Werror

.PHONY: all clean

all: nvq_model_bench

nvq_model_bench: nvq_model_bench.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f nvq_model_bench
