/**
 * @file makefile_loader.c
 * 
 * @brief Implementation of makefile loading functions.
 * 
 * This source file implements functions for loading the makefile for the mmake program,
 * including handling default makefile names and parsing the makefile content.
 * 
 * Error handling: Errors in file operations or parsing result in error messages and program exit.
 * 
 * Memory management: Allocated memory for the makefile is freed during the clean-up phase.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "makefile_loader.h"
#include "parser.h"

#define DEFAULT_MAKEFILE "mmakefile"

makefile *load_makefile(options_t *options) {
    const char *makefile_path;

    if (options->makefile_name) {
        makefile_path = options->makefile_name;
    } else {
        makefile_path = DEFAULT_MAKEFILE;
    }

    FILE *fp = fopen(makefile_path, "r");
    if (!fp) {
        fprintf(stderr, "mmakefile: No such file or directory\n");
        exit(EXIT_FAILURE);
    }

    makefile *make = parse_makefile(fp);
    if (!make) {
        fprintf(stderr, "mmakefile: Could not parse %s\n", makefile_path);
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    return make; 
}

void clean_up(makefile *make, options_t *options) {
    makefile_del(make);
    free(options->makefile_name);
}
