/**
 * file: pipes.c
 * 
 * This module provides functions for creating pipes for communication between 
 * commands, handling setup, and closing all file descriptors associated with the pipes.
 * 
 * Error handling: This module handles errors by printing error messages using `perror` 
 * and immediately exiting the program on critical failures such as memory allocation or pipe creation errors.
 * 
 * Memory management: You need to manually free the memory allocated for the pipes using `close_pipes`.
 * 
 * @date 14-10-2024
 * @author Emil Engvall
 */

#include "pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int **setup_pipes(int num_pipes) {
    if (num_pipes <= 0) {
        return NULL;
    }
    
    int **pipes = malloc(num_pipes * sizeof(int *));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_pipes; i++) {
        pipes[i] = malloc(2 * sizeof(int));
        if (pipes[i] == NULL) {
            perror("malloc");
            for (int j = 0; j < i; j++) {
                free(pipes[j]);
            }
            free(pipes);
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            for (int j = 0; j <= i; j++) {
                free(pipes[j]);
            }
            free(pipes);
            exit(EXIT_FAILURE);
        }
    }

    return pipes;
}

void close_pipes(int **pipes, int num_pipes) {
    if (!pipes) {
        return;
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
        free(pipes[i]);
    }
    
    free(pipes);
}
