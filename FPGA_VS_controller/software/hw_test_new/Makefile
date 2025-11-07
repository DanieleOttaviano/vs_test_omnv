CC ?= gcc
ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

all: hw_test

hw_test: hw_test.c
	$(CROSS_COMPILE)$(CC) -I.. -o hw_test hw_test.c -lm

clean:
	rm hw_test
