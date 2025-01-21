/**
 * file: command.c
 * 
 * This module provides functions to execute commands, handle 
 * input/output redirection, and manage processes.
 * 
 * Error handling: Errors are managed by printing messages with perror and exiting 
 * on critical failures, such as errors in fork, dup2, or exec.
 * 
 * Memory management: The memory for commands needs to be manually freed by calling free_commands.
 * 
 * date: 14-10-2024
 * author: Emil Engvall
 */

#include "command.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h> 
#include <errno.h> 

int execute_commands(char ***commands, int cmd_count, int **pipes) {
    int *pids = malloc(cmd_count * sizeof(int));
    if (!pids) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int exit_status = 0;

    for (int i = 0; i < cmd_count; i++) {
        fork_and_execute_command(i, commands, cmd_count, pipes, pids);
    }

    close_all_pipes(pipes, cmd_count);
    wait_for_children(pids, cmd_count, &exit_status);

    free(pids);
    return exit_status;
}

void fork_and_execute_command(int i, char ***commands, int cmd_count, int **pipes, int *pids) {
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        setup_redirection(i, cmd_count, pipes);
        close_all_pipes(pipes, cmd_count);
        execvp(commands[i][0], commands[i]);
        fprintf(stderr, "%s: %s\n", commands[i][0], strerror(errno));
        exit(EXIT_FAILURE);
    }

    pids[i] = pid;
}

void wait_for_children(int *pids, int cmd_count, int *exit_status) {
    for (int i = 0; i < cmd_count; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid");
            *exit_status = EXIT_FAILURE;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            *exit_status = EXIT_FAILURE;
        } else if (WIFSIGNALED(status)) {
            *exit_status = EXIT_FAILURE;
        }
    }
}

void setup_redirection(int i, int cmd_count, int **pipes) {
    if (i > 0) {
        if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }

    if (i < cmd_count - 1) {
        if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }
}

void close_all_pipes(int **pipes, int cmd_count) {
    for (int j = 0; j < cmd_count - 1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
}
