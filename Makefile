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

CC=gcc

ODIR=obj
SRCDIR=src
BINDIR=bin

CFLAGS = -ggdb -fsanitize=address
IFLAGS = -Iraylib/src -Icomponents -isystem include
LDFLAGS = -L./lib
LDLIBS = -lraylib
CHECKFLAGS = -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough \
-D_GLIBCXX_ASSERTIONS \
-fstack-protector-strong \
-Werror=implicit -Werror=incompatible-pointer-types -Werror=int-conversion \

ifeq ($(PLATFORM),OSX)
	LDFLAGS += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
endif

ifeq ($(BUILD),RELEASE)
	CFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -o2
else
	CFLAGS += $(CHECKFLAGS)
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
	rm -f $(ODIR)/*/*.o
	rm -f $(ODIR)/*.o