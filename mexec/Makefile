#Author: Emil Engvall
#Date: 14-10-2024

CC = gcc
CFLAGS = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic \
         -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition

# Source files
SRCS = mexec.c parser.c pipes.c command.c

# Object files
OBJS = mexec.o parser.o pipes.o command.o

# Header files
HEADERS = parser.h pipes.h command.h

# Default target
all: mexec

# Linking object files to create the executable program
mexec: $(OBJS)
	$(CC) $(CFLAGS) -o mexec $(OBJS)

# Compiling mexec.c
mexec.o: mexec.c $(HEADERS)
	$(CC) $(CFLAGS) -c mexec.c -o mexec.o

# Compiling parser.c
parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o parser.o

# Compiling pipes.c
pipes.o: pipes.c pipes.h
	$(CC) $(CFLAGS) -c pipes.c -o pipes.o

# Compiling command.c
command.o: command.c command.h
	$(CC) $(CFLAGS) -c command.c -o command.o

# Cleaning up compiled files
clean:
	rm -f $(OBJS) mexec

