/**
 * @file mdu.c
 * @brief Main program for calculating the disk usage of files and directories.
 *
 * This file contains the main function which parses the command-line arguments,
 * initializes the necessary worker threads, and manages the disk usage calculation.
 * It supports parallel processing using the `-j` flag to specify the number of threads.
 * 
 * Memory management: Memory for paths and thread data is dynamically allocated and cleaned up.
 * 
 * Error handling: Invalid arguments or failures to start threads will result in appropriate error messages
 * and program termination.
 * 
 * @author Emil Engvall
 * @date 21-10-2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "thread.h"
#include "worker.h"

static char **parse_paths(int argc, char *argv[], int *num_paths, int *exit_code);
static int start_worker_threads(int num_threads, pthread_t **threads, struct ThreadData *data);

int main(int argc, char *argv[]) {
    int num_threads = 1;
    int opt;
    int exit_code = EXIT_SUCCESS;

    while ((opt = getopt(argc, argv, "j:")) != -1) {
        if (opt == 'j') {
            num_threads = atoi(optarg);
            if (num_threads < 1) {
                fprintf(stderr, "Invalid number of threads: %s\n", optarg);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Usage: %s [-j num_threads] file ...\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int num_paths = 0;
    char **paths = parse_paths(argc, argv, &num_paths, &exit_code);
    if (!paths) {
        exit(exit_code);
    }

    for (int i = 0; i < num_paths; i++) {
        struct ThreadData *data = malloc(sizeof(struct ThreadData));
        if (!data) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        if (thread_data_init(data) != 0) {
            free(data);
            exit(EXIT_FAILURE);
        }

        data->pending = 1;
        char *path = strdup(paths[i]);
        if (!path) {
            perror("strdup");
            thread_data_destroy(data);
            free(data);
            exit(EXIT_FAILURE);
        }

        if (enqueue(data, path) != 0) {
            perror("enqueue");
            free(path);
            thread_data_destroy(data);
            free(data);
            exit(EXIT_FAILURE);
        }

        pthread_t *threads = NULL;
        if (start_worker_threads(num_threads, &threads, data) != 0) {
            thread_data_destroy(data);
            free(data);
            exit(EXIT_FAILURE);
        }

        for (int t = 0; t < num_threads; t++) {
            int ret = pthread_join(threads[t], NULL);
            if (ret != 0) {
                fprintf(stderr, "pthread_join failed: %s\n", strerror(ret));
                exit(EXIT_FAILURE);
            }
        }

        if (data->error_occurred) {
            exit_code = EXIT_FAILURE;
        }

        long total_blocks = (data->total_size + 511) / 512;
        printf("%ld\t%s\n", total_blocks, paths[i]);

        free(threads);
        thread_data_destroy(data);
        free(data);
    }

    if (paths != argv + optind) {
        free(paths);
    }

    return exit_code;
}


/**
 * @brief Parses the file paths from the command-line arguments.
 *
 * This function extracts the file paths specified as arguments after parsing the options.
 * 
 * @param argc The argument count.
 * @param argv The argument vector.
 * @param num_paths Pointer to an integer where the number of paths will be stored.
 * @param exit_code Pointer to an integer where the exit code will be stored if an error occurs.
 * @return A dynamically allocated array of strings containing the file paths.
 */
static char **parse_paths(int argc, char *argv[], int *num_paths, int *exit_code) {
    int paths_size = 0;
    int paths_count = 0;
    char **paths = NULL;

    for (int i = optind; i < argc; i++) {
        if (paths_count == paths_size) {
            if (paths_size == 0) {
                paths_size = 10; 
            } else {
                paths_size *= 2;
            }

            char **tmp = realloc(paths, paths_size * sizeof(char *));
            if (!tmp) {
                perror("realloc");
                free(paths);
                *exit_code = EXIT_FAILURE;
                return NULL;
            }
            paths = tmp;
        }

        paths[paths_count++] = argv[i];
    }

    if (paths_count == 0) {
        paths_count = 1;
        paths = malloc(sizeof(char *));
        if (!paths) {
            perror("malloc");
            *exit_code = EXIT_FAILURE;
            return NULL;
        }
        paths[0] = ".";
    }

    *num_paths = paths_count;
    return paths;
}

/**
 * @brief Starts the worker threads for parallel processing.
 *
 * This function initializes and starts the worker threads based on the specified
 * number of threads. Each thread will process a portion of the file paths to calculate
 * the disk usage.
 * 
 * @param num_threads The number of threads to create.
 * @param threads Pointer to an array of thread handles.
 * @param data Pointer to the thread data structure.
 * @return 0 on success, or a non-zero error code on failure.
 */
static int start_worker_threads(int num_threads, pthread_t **threads, struct ThreadData *data) {
    *threads = malloc(num_threads * sizeof(pthread_t));
    if (!*threads) {
        perror("malloc");
        return -1;
    }

    for (int t = 0; t < num_threads; t++) {
        int ret = pthread_create(&(*threads)[t], NULL, worker_thread, data);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
            free(*threads);
            return -1;
        }
    }
    return 0;
}
