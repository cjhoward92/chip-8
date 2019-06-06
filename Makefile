SRCDIR=src
OUTDIR=bin
IDIR=./$(SRCDIR)

CC=gcc
CFLAGS=-g -I$(IDIR) -Wall

_DEPS=chip8.h stdhdr.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=chip8.o main.o
OBJ=$(patsubst %,$(OUTDIR)/%,$(_OBJ))

$(OUTDIR)/chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(DEPS) $(OUTDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTDIR):
	mkdir -p $(OUTDIR)

.PHONY: clean
.PHONY: print

print:
	echo $(DEPS)

clean:
	rm -f $(OUTDIR)/*.o chip8