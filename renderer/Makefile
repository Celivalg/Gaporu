CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2 -g -fsanitize=address 
CPPFLAGS= -MMD

LDFLAGS =

OBJDIR = ./obj
TESTSRCDIR = ./unit-tests
TESTOBJDIR = ./unit-tests/obj
TESTSRC = $(wildcard $(TESTSRCDIR)/*.c)
TESTOBJ = $(patsubst $(TESTSRCDIR)/%.c,$(TESTOBJDIR)/%.o,$(TESTSRC))
SRCDIR = ./src
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
DEP := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRC))
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

.PHONY: clean all $(current_dir)



all: $(current_dir)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
-include $(DEP)

$(TESTOBJDIR)/%.o: $(TESTSRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

${current_dir}: $(OBJ)


unit-test: $(OBJ) $(TESTOBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ) $(TESTOBJ) -o unit-test

#-shared


clean:
	${RM} ${OBJ} ${DEP}



# END
