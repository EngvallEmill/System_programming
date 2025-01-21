/**
 * @file build.c
 * 
 * @brief Implementation of target building functions.
 * 
 * This source file implements functions for building the specified targets in the makefile.
 * It handles dependency resolution, determining if a target needs to be built, and executing commands.
 * 
 * Error handling: Errors during the build process result in error messages and program exit.
 * 
 * Memory management: Resources are freed after the build process completes or upon encountering errors.
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
#include "parser.h"

void build_targets(makefile *make, options_t *options) {
    if (options->target_count == 0) {
        const char *default_target = makefile_default_target(make);
        if (!default_target) {
            handle_no_default_target(make);
        }
        options->targets = (char **)&default_target;
        options->target_count = 1;
    }

    for (int i = 0; i < options->target_count; i++) {
        if (process_target(make, options->targets[i], options) != 0) {
            makefile_del(make);
            exit(EXIT_FAILURE);
        }
    }
}

int process_target(makefile *make, const char *target_name, options_t *options) {
    rule *r = makefile_rule(make, target_name);

    if (!r && file_exists(target_name)) {
        return 0;
    }

    if (r) {
        if (build_prerequisites(make, r, options) != 0) {
            return -1;
        }
    } else {
        handle_missing_rule(target_name);
    }

    int build = needs_build(make, target_name, options);
    if (build == 1) {
        char **cmd = rule_cmd(r);
        if (!cmd || !cmd[0]) {
            handle_no_command(target_name);
        }
        if (!options->silent) {
            print_command(cmd);
        }
        if (execute_command(cmd) != 0) {
            return -1;
        }
    }
    return 0;
}

