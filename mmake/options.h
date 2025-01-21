/**
 * @file options.h
 * 
 * @brief Command-line option parsing for the mmake program.
 * 
 * This header file declares the structure and functions related to command-line options
 * for the mmake program, including parsing arguments and displaying usage information.
 * 
 * Error handling: Errors are reported via `stderr` and the program may exit on critical failures.
 * 
 * Memory management: Dynamically allocated memory for option strings must be freed appropriately.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#ifndef OPTIONS_H
#define OPTIONS_H

/**
 * @brief Structure to hold command-line options.
 */
typedef struct {
    int force_build;     /**< Flag to force build of all targets. */
    int silent;          /**< Flag to suppress command output. */
    char *makefile_name; /**< Name of the makefile to use. */
    int target_count;    /**< Number of targets specified. */
    char **targets;      /**< Array of target names. */
} options_t;

/**
 * @brief Parses command-line arguments.
 * 
 * This function processes the command-line arguments and returns a populated `options_t` structure.
 * 
 * @param argc Argument count from `main`.
 * @param argv Argument vector from `main`.
 * @return An `options_t` structure containing the parsed options.
 */
options_t parse_arguments(int argc, char *argv[]);

/**
 * @brief Displays usage information for the mmake program.
 * 
 * @param progname The name of the program (usually `argv[0]`).
 */
void usage(const char *progname);

#endif
