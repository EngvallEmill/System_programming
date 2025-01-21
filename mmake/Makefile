CC = gcc
CFLAGS = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic \
         -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition

OBJS = mmake.o options.o makefile_loader.o build.o utils.o parser.o

mmake: $(OBJS)
	$(CC) $(CFLAGS) -o mmake $(OBJS)

mmake.o: mmake.c options.h makefile_loader.h build.h utils.h parser.h
	$(CC) $(CFLAGS) -c mmake.c

options.o: options.c options.h parser.h
	$(CC) $(CFLAGS) -c options.c

makefile_loader.o: makefile_loader.c makefile_loader.h options.h parser.h
	$(CC) $(CFLAGS) -c makefile_loader.c

build.o: build.c build.h options.h makefile_loader.h utils.h parser.h
	$(CC) $(CFLAGS) -c build.c

utils.o: utils.c utils.h options.h makefile_loader.h build.h parser.h
	$(CC) $(CFLAGS) -c utils.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

clean:
	rm -f mmake $(OBJS)
