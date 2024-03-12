PLATFORM =
ifeq ($(OS),Windows_NT)
    PLATFORM=WIN32
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = OSX
    endif
endif

BUILD=DEBUG
CC=gcc

ODIR=obj
SRCDIR=src
BINDIR=bin

CFLAGS = -ggdb -fsanitize=address -O2
IFLAGS = -Iraylib/src
LDFLAGS =
LDLIBS = -lraylib
CHECKFLAGS = -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
-D_GLIBCXX_ASSERTIONS \
-fstack-clash-protection -fstack-protector-strong \
-Wl,-z,nodlopen -Wl,-z,noexecstack \
-Wl,-z,relro -Wl,-z,now \
-Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion \

ifeq ($(PLATFORM),OSX)
    LDFLAGS += -L./raylib/src -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
endif

SRCS := $(wildcard $(SRCDIR)/*.c)
SRCS += $(wildcard $(SRCDIR)/*/*.c)
_OBJ := $(patsubst $(SRCDIR)/%.c,%.o,$(SRCS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(IFLAGS) $(CFLAGS) $(PLATFLAGS)

eschatology:$(OBJ)
	$(CC) -o $(BINDIR)/eschatology $(OBJ) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(PLATFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o