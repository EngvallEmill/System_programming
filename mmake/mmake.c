/**
 * @file mmake.c
 * 
 * @brief Entry point for the mmake program.
 * 
 * This is the main entry point for the mmake program. It handles the initialization,
 * loading of the makefile, building targets, and clean-up after the build process.
 * 
 * Error handling: Critical errors cause the program to exit with an error status.
 * 
 * Memory management: Allocated resources are freed during the clean-up phase.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "makefile_loader.h"
#include "build.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    options_t options = parse_arguments(argc, argv);
    makefile *make = load_makefile(&options);
    build_targets(make, &options);
    clean_up(make, &options);
    return EXIT_SUCCESS;
}

