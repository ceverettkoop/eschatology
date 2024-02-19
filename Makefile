PLATFORM ?= DESKTOP
BUILD ?= DEBUG
CC=gcc

ODIR=obj
SRCDIR=src
BINDIR=bin

IFLAGS = -I../include -I../libparseehx/src
CFLAGS = -ggdb
LDFLAGS =
LDLIBS = -lraylib
PLATFLAGS = 

SRCS := $(wildcard $(SRCDIR)/*.c)
_OBJ := $(patsubst $(SRCDIR)/%.c,%.o,$(SRCS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(IFLAGS) $(CFLAGS) $(PLATFLAGS)

eschatology:$(OBJ)
	$(CC) -o $(BINDIR)/eschatology $(OBJ) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(PLATFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o