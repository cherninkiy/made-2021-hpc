CC=gcc
CFLAGS=-Wall
LDFLAGS=
INCLUDE_PATH=/usr/include/x86_64-linux-gnu
LIBRARY_PATH=/usr/lib/x86_64-linux-gnu/blas
SOURCES=main.c matmul.c matvec.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=matmul

all: $(TARGET)

$(TARGET): 
	$(CC) *.c $(CFLAGS) -lblas -o $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

optim: CFLAGS += -O3
optim: $(TARGET)

fast: CFLAGS += -Ofast
fast: $(TARGET)

tests:
	./tests.sh host

clean:
	rm -rf *.o $(TARGET)