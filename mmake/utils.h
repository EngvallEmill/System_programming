/**
 * @file utils.h
 * 
 * @brief Utility functions for the mmake program.
 * 
 * This header file declares utility functions used throughout the mmake program, such as
 * checking file existence, retrieving modification times, determining if a build is needed,
 * executing commands, printing commands, building prerequisites, and handling error scenarios.
 * 
 * Error handling: Functions report errors via `stderr` and may exit the program on critical failures.
 * 
 * Memory management: Functions that allocate memory must be properly freed by the caller.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "parser.h"
#include "options.h"

/**
 * @brief Checks if a file exists at the given path.
 * 
 * @param path The file path to check.
 * @return Non-zero if the file exists, zero otherwise.
 */
int file_exists(const char *path);

/**
 * @brief Retrieves the modification time of a file.
 * 
 * @param path The file path to check.
 * @return The modification time as `time_t`, or 0 if the file doesn't exist.
 */
time_t get_mod_time(const char *path);

/**
 * @brief Determines if the target needs to be built.
 * 
 * @param make The makefile structure containing rules.
 * @param target_name The name of the target to check.
 * @param options The options structure containing command-line options.
 * @return 1 if the target needs to be built, 0 otherwise.
 */
int needs_build(makefile *make, const char *target_name, options_t *options);

/**
 * @brief Executes a command using `execvp`.
 * 
 * @param cmd The command array to execute.
 * @return 0 on success, -1 on failure.
 */
int execute_command(char **cmd);

/**
 * @brief Prints a command to `stdout`.
 * 
 * @param cmd The command array to print.
 */
void print_command(char **cmd);

/**
 * @brief Builds all prerequisites for a given rule.
 * 
 * @param make The makefile structure containing rules.
 * @param r The rule whose prerequisites need to be built.
 * @param options The options structure containing command-line options.
 * @return 0 on success, -1 on failure.
 */
int build_prerequisites(makefile *make, rule *r, options_t *options);

/**
 * @brief Handles the error when a rule is missing for a target.
 * 
 * @param target_name The name of the missing target.
 */
void handle_missing_rule(const char *target_name);

/**
 * @brief Handles the error when no command is specified for a target.
 * 
 * @param target_name The name of the target with no command.
 */
void handle_no_command(const char *target_name);

/**
 * @brief Handles the error when no default target is found.
 * 
 * @param make The makefile structure.
 */
void handle_no_default_target(makefile *make);

#endif 
