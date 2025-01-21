/**
 * file: parser.c
 * 
 * This module provides functions for parsing command lines and tokenizing 
 * them into individual arguments, trimming whitespace, and handling multiple commands.
 * 
 * Error handling: Errors are handled by printing appropriate messages using perror 
 * and exiting the program on memory allocation failures or invalid input.
 * 
 * date: 14-10-2024
 * author: Emil Engvall
 */

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void trim(char *str) {
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0';

    if (start != str) {
        memmove(str, start, end - start + 1);
    }
}

char **parse_command(const char *line, int *argc_out) {
    int argc = 0;
    int capacity = INITIAL_ARG_CAPACITY;

    char **argv = malloc(capacity * sizeof(char *));
    if (!argv) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *copy = strdup(line);
    if (!copy) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *ptr = copy;
    char *token;

    while ((token = strtok_r(ptr, " \t\n", &ptr))) {
        if (argc >= capacity) {
            capacity *= 2;
            argv = realloc(argv, capacity * sizeof(char *));
            if (!argv) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        argv[argc++] = strdup(token);
    }

    argv[argc] = NULL;
    if (argc_out) {
        *argc_out = argc;
    }

    free(copy);
    return argv;
}

char ***allocate_commands(void) {
    char ***commands = malloc(INITIAL_CMD_CAPACITY * sizeof(char **));
    if (!commands) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return commands;
}

int read_and_trim_line(FILE *input, char *line) {
    while (fgets(line, MAX_LINE_LENGTH, input)) {
        trim(line);
        if (line[0] != '\0') {
            return 1;
        }
    }
    return 0;
}

char ***add_command(char ***commands, int *cmd_count, int *cmd_capacity, char **args) {
    if (*cmd_count >= *cmd_capacity) {
        *cmd_capacity *= 2;
        char ***temp = realloc(commands, (*cmd_capacity) * sizeof(char **));
        if (!temp) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        commands = temp;
    }
    commands[(*cmd_count)++] = args;
    return commands;
}

char ***read_commands(FILE *input, int *cmd_count_out) {
    char ***commands = allocate_commands();
    int cmd_capacity = INITIAL_CMD_CAPACITY;
    int cmd_count = 0;
    char line[MAX_LINE_LENGTH];

    while (read_and_trim_line(input, line)) {
        int argc;
        char **args = parse_command(line, &argc);
        commands = add_command(commands, &cmd_count, &cmd_capacity, args);
    }

    *cmd_count_out = cmd_count;
    return commands;
}

void free_commands(char ***commands, int cmd_count) {
    for (int i = 0; i < cmd_count; i++) {
        char **cmd = commands[i];
        for (int j = 0; cmd[j] != NULL; j++) {
            free(cmd[j]);
        }
        free(cmd);
    }
    free(commands);
}
