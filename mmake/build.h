/**
 * @file build.h
 * 
 * @brief Functions for building targets specified in the makefile.
 * 
 * This header file declares functions for building targets, handling the build process,
 * and processing individual targets based on their dependencies and commands.
 * 
 * Error handling: Functions report errors via `stderr` and may exit on critical failures.
 * 
 * Memory management: Resources allocated during the build process are freed appropriately.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#ifndef BUILD_H
#define BUILD_H

#include "parser.h"
#include "options.h"

/**
 * @brief Builds all specified targets in the makefile.
 * 
 * This function initiates the build process for the targets specified in the command-line options.
 * If no targets are specified, it builds the default target.
 * 
 * @param make The makefile structure containing rules and targets.
 * @param options The options structure containing command-line options.
 */
void build_targets(makefile *make, options_t *options);

/**
 * @brief Processes an individual target, building it if necessary.
 * 
 * This function checks if a target needs to be built, processes its prerequisites,
 * and executes the associated commands if a build is needed.
 * 
 * @param make The makefile structure containing rules.
 * @param target_name The name of the target to process.
 * @param options The options structure containing command-line options.
 * @return 0 on success, -1 on failure.
 */
int process_target(makefile *make, const char *target_name, options_t *options);

#endif
