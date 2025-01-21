/**
 * file: mexec.c
 * 
 * This program reads a list of commands from either standard input or a file 
 * provided as an argument, then executes the commands sequentially or as a pipeline 
 * if multiple commands are provided. The program creates pipes for inter-process communication, 
 * sets up child processes for each command, and connects their input and output streams.
 * 
 * Error handling: Errors are handled by printing messages with perror and exiting 
 * the program in the case of failures such as file opening, memory allocation, 
 * fork, pipe, or exec errors.
 * 
 * Memory management: The memory allocated for pipes needs to be manually freed using 
 * the close_pipes function, and the memory allocated for commands should be freed 
 * using free_commands.
 * 
 * date: 14-10-2024
 * author: Emil Engvall
 */

#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "pipes.h"
#include "command.h"
#include "parser.h"

FILE *open_input_file(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    FILE *input = open_input_file(argc, argv);

    int cmd_count;
    char ***commands = read_commands(input, &cmd_count);

    if (input != stdin) {
        fclose(input);
    }

    if (cmd_count == 0) {
        free_commands(commands, cmd_count);
        exit(EXIT_SUCCESS);
    }

    int num_pipes = cmd_count - 1;
    int **pipes_ptr = setup_pipes(num_pipes);

    int overall_exit_status = execute_commands(commands, cmd_count, pipes_ptr);

    free_commands(commands, cmd_count);
    close_pipes(pipes_ptr, num_pipes);

    return overall_exit_status;
}

FILE *open_input_file(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "usage: %s [FILE]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "%s: No such file or directory\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        return file;
    }

    return stdin;
}