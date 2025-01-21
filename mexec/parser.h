#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

/**
 * @file parser.h
 * @brief Declarations for command line parsing functions.
 * 
 * This module provides functions for parsing command lines and tokenizing 
 * them into individual arguments, trimming whitespace, and handling multiple commands.
 * 
 * Error handling: Errors are handled by printing appropriate messages using perror 
 * and exiting the program on memory allocation failures or invalid input.
 * 
 * @date 14-10-2024
* @author Emil Engvall
 */

#define MAX_LINE_LENGTH 1024
#define INITIAL_ARG_CAPACITY 10
#define INITIAL_CMD_CAPACITY 10

/**
 * @brief Trims leading and trailing whitespace from a string.
 * 
 * This function removes any leading or trailing spaces from the input string.
 * 
 * @param str The string to trim.
 */
void trim(char *str);

/**
 * @brief Parses a command line into arguments.
 * 
 * This function takes a command line string and splits it into individual 
 * arguments, returning a dynamically allocated array of arguments.
 * 
 * @param line The command line to parse.
 * @param argc_out Pointer to an integer where the argument count will be stored.
 * @return A NULL-terminated array of strings representing the arguments.
 */
char **parse_command(const char *line, int *argc_out);

/**
 * @brief Reads all commands from input.
 * 
 * This function reads multiple commands from the input stream and returns them as 
 * a dynamically allocated array of commands.
 * 
 * @param input The input file to read commands from.
 * @param cmd_count_out Pointer to an integer where the number of commands will be stored.
 * @return A dynamically allocated array of commands.
 */
char ***read_commands(FILE *input, int *cmd_count_out);

/**
 * @brief Reads a single line from input and trims it.
 * 
 * This function reads a line from the input file, trims any leading and trailing whitespace,
 * and returns 1 if a non-empty line was read, or 0 otherwise.
 * 
 * @param input The input file to read from.
 * @param line Buffer to store the read line.
 * @return 1 if a non-empty line was read, 0 otherwise.
 */
int read_and_trim_line(FILE *input, char *line);

/**
 * @brief Allocates initial memory for the commands array.
 * 
 * This function allocates memory for the commands array with an initial capacity.
 * 
 * @return A pointer to the newly allocated commands array.
 */
char ***allocate_commands(void);

/**
 * @brief Adds a command to the commands array, resizing if necessary.
 * 
 * This function adds a new command to the commands array, and reallocates memory if the capacity is exceeded.
 * 
 * @param commands The current commands array.
 * @param cmd_count Pointer to the current command count.
 * @param cmd_capacity Pointer to the current capacity of the commands array.
 * @param args The command arguments to add.
 * @return The (possibly reallocated) commands array.
 */
char ***add_command(char ***commands, int *cmd_count, int *cmd_capacity, char **args);

/**
 * @brief Frees the memory allocated for the commands array.
 * 
 * This function frees all memory associated with the commands array.
 * 
 * @param commands The commands array to free.
 * @param cmd_count The number of commands in the array.
 */
void free_commands(char ***commands, int cmd_count);

#endif // PARSER_H
