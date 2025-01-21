/**
 * @file makefile_loader.h
 * 
 * @brief Functions for loading and cleaning up the makefile.
 * 
 * This header file declares functions for loading and cleaning up the makefile used by the mmake program.
 * 
 * Error handling: Functions report errors via `stderr` and may exit on critical failures.
 * 
 * Memory management: The loaded makefile must be properly freed using the provided clean-up functions.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#ifndef MAKEFILE_LOADER_H
#define MAKEFILE_LOADER_H

#include "parser.h"
#include "options.h"

/**
 * @brief Loads the makefile specified in the options.
 * 
 * If no makefile is specified, it attempts to load a default makefile.
 * 
 * @param options The options structure containing command-line options.
 * @return A pointer to the loaded makefile structure.
 */
makefile *load_makefile(options_t *options);

/**
 * @brief Cleans up allocated resources after the build process.
 * 
 * @param make The makefile structure to clean up.
 * @param options The options structure to clean up.
 */
void clean_up(makefile *make, options_t *options);

#endif
