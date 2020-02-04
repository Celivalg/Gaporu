CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2 -g -fsanitize=address 

LDFLAGS =
LDLIBS= -l OpenCL



all: fractalviewer

.PHONY: clean fractalviewer

fractalviewer: 
	+$(MAKE) -C dir1 all
	+$(MAKE) -C dir2 all

unit-test:
	+$(MAKE) -C dir1 unit-test
	+$(MAKE) -C dir2 unit-test

clean:
	+$(MAKE) -C dir1 clean
	+$(MAKE) -C dir2 clean

# END
