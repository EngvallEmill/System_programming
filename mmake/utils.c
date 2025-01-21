/**
 * @file utils.c
 * 
 * @brief Utility function implementations for the mmake program.
 * 
 * This source file implements utility functions declared in `utils.h`, providing functionality for
 * checking file existence, retrieving modification times, determining if a build is needed,
 * executing commands, printing commands, building prerequisites, and handling error conditions.
 * 
 * Error handling: Functions report errors via `stderr` and exit the program on critical failures.
 * 
 * Memory management: Allocated memory should be freed by the caller where applicable.
 * 
 * @author Emil Engvall
 * @date 31-10-2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "options.h"
#include "makefile_loader.h"
#include "build.h"
#include "utils.h"

int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

time_t get_mod_time(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        return 0;
    }
    return st.st_mtime;
}

int needs_build(makefile *make, const char *target_name, options_t *options) {
    if (options->force_build) {
        return 1;
    }

    time_t target_time = get_mod_time(target_name);
    if (target_time == 0) {
        return 1;
    }

    rule *r = makefile_rule(make, target_name);
    if (!r) {
        return 0;
    }

    const char **prereqs = rule_prereq(r);
    for (const char **p = prereqs; *p; p++) {
        const char *prereq = *p;
        time_t prereq_time = get_mod_time(prereq);

        if (prereq_time == 0) {
            fprintf(stderr, "mmake: Prerequisite '%s' for target '%s' does not exist\n", prereq, target_name);
            exit(EXIT_FAILURE);
        }

        if (prereq_time > target_time) {
            return 1;
        }
    }
    return 0;
}

int execute_command(char **cmd) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("mmake: fork");
        return -1;
    } else if (pid == 0) {
        execvp(cmd[0], cmd);
        perror("mmake: execvp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("mmake: waitpid");
            return -1;
        }
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != EXIT_SUCCESS) {
                exit(EXIT_FAILURE); 
            }
        } else if (WIFSIGNALED(status)) {
            exit(EXIT_FAILURE);
        } else {
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void print_command(char **cmd) {
    for (char **c = cmd; *c; c++) {
        printf("%s", *c);
        if (*(c + 1)) {
            printf(" ");
        }
    }
    printf("\n");
}

int build_prerequisites(makefile *make, rule *r, options_t *options) {
    const char **prereqs = rule_prereq(r);
    for (const char **p = prereqs; *p; p++) {
        if (process_target(make, *p, options) != 0) {
            return -1;
        }
    }
    return 0;
}


void handle_missing_rule(const char *target_name) {
    fprintf(stderr, "mmake: No rule to make target '%s'\n", target_name);
    exit(EXIT_FAILURE);
}

void handle_no_command(const char *target_name) {
    fprintf(stderr, "mmake: No command specified for target '%s'\n", target_name);
    exit(EXIT_FAILURE);
}

void handle_no_default_target(makefile *make) {
    fprintf(stderr, "mmake: No targets specified and no default target found\n");
    makefile_del(make);
    exit(EXIT_FAILURE);
}
