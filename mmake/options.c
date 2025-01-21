/**
 * @file options.c
 * 
 * @brief Command-line option parsing implementation for the mmake program.
 * 
 * This source file implements functions for parsing command-line options for the mmake program.
 * It handles options such as specifying a makefile, forcing builds, and setting silent mode.
 * 
 * Error handling: Errors during parsing are reported via `stderr`, and the program exits on critical failures.
 * 
 * Memory management: Allocated memory for option strings is freed in the clean-up phase.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "options.h"

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-f MAKEFILE] [-B] [-s] [TARGET ...]\n", progname);
}

options_t parse_arguments(int argc, char *argv[]) {
    options_t options = {0, 0, NULL, 0, NULL};
    int opt;

    while ((opt = getopt(argc, argv, "f:Bs")) != -1) {
        switch (opt) {
            case 'f':
                options.makefile_name = strdup(optarg);
                if (!options.makefile_name) {
                    perror("mmake: strdup");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'B':
                options.force_build = 1;
                break;
            case 's':
                options.silent = 1;
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    options.target_count = argc - optind;
    if (options.target_count > 0) {
        options.targets = &argv[optind];
    }

    return options;
}
