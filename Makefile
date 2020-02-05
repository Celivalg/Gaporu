CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2 -g -fsanitize=address 

LDFLAGS =
LDLIBS= -l OpenCL



all: gaporu

.PHONY: clean gaporu

gaporu: 
	+$(MAKE) -C renderer all

unit-test:
	+$(MAKE) -C renderer unit-test

clean:
	+$(MAKE) -C rederer clean

# END
