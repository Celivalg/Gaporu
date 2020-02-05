CC=gcc
CFLAGS= -Wall -Wextra -std=c99 -O2 -g
CPPFLAGS= -MMD

LDOPENCL = -l OpenCL 
LDGTK = -lm `pkg-config gtk+-3.0 --cflags --libs`

OBJDIR = ./obj
TESTSRCDIR = ./unit-tests
TESTOBJDIR = ./unit-tests/obj
TESTSRC = $(wildcard $(TESTSRCDIR)/*.c)
TESTOBJ = $(patsubst $(TESTSRCDIR)/%.c,$(TESTOBJDIR)/%.o,$(TESTSRC))
TESTDEP = $(patsubst $(TESTSRCDIR)/%.c,$(TESTOBJDIR)/%.d,$(TESTSRC))
SRCDIR = ./src
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
DEP := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRC))
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

.PHONY: clean all $(current_dir)



all: $(current_dir)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDGTK) -c $< -o $@
-include $(DEP)

$(TESTOBJDIR)/%.o: $(TESTSRCDIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDOPENCL) $(LDGTK) -c $< -o $@

${current_dir}: $(OBJ)


unit-test: $(OBJ) $(TESTOBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ) $(TESTOBJ) -o unit-test $(LDOPENCL) $(LDGTK)

#-shared


clean:
	$(RM) $(OBJ) $(DEP) $(TESTOBJ) $(TESTDEP) unit-test



# END
