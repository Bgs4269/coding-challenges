
CC=gcc
CLIBS=
LDFLAGS=-lm

#CFLAGS=-O0 -g -Wall -Wextra
CFLAGS=-O2 -Wall

OBJS=factor.o
BIN=factor

TESTNUM=718061908054592475

all: $(BIN)
	@strip $(BIN)

factor.o:
	$(CC) -c $(CFLAGS) -o factor.o factor.c

factor: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)

clean:
	rm -f $(OBJS) $(BIN)

debug: $(OBJ)
	gdb --args ./$(BIN) $(TESTNUM)
