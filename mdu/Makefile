CC = gcc
CFLAGS = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic \
         -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition

LDFLAGS = -lm -pthread

OBJ = mdu.o thread.o worker.o

all: mdu

mdu: $(OBJ)
	$(CC) $(LDFLAGS) -o mdu $(OBJ)

mdu.o: mdu.c thread.h worker.h
	$(CC) $(CFLAGS) -c mdu.c

thread.o: thread.c thread.h
	$(CC) $(CFLAGS) -c thread.c

worker.o: worker.c worker.h thread.h
	$(CC) $(CFLAGS) -c worker.c

clean:
	rm -f mdu $(OBJ)

.PHONY: all clean
