/**
 * @file worker.c
 * @brief Worker thread implementation for processing file paths.
 *
 * This file contains the worker thread logic that processes file and directory paths to calculate
 * disk usage. It includes functions to handle files, directories, and thread-safe operations using
 * mutexes and condition variables.
 *
 * Error handling: If any errors occur during processing, they are updated in the shared thread data structure.
 * 
 * Memory management: Memory for paths and file statistics is dynamically allocated and freed appropriately.
 * 
 * @author Emil Engvall
 * @date 21-10-2024
 */

#include "worker.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

/* ------------------ Declarations of internal functions ------------------ */

static void process_path(char *path, struct ThreadData *data);
static void handle_file(const struct stat *st, struct ThreadData *data);
static void handle_directory(const char *path, struct ThreadData *data);
static char *construct_path(const char *dir_path, const char *entry_name);
static void update_error_status(struct ThreadData *data, int error_in_this_call);


/* -------------------------- External functions -------------------------- */

void *worker_thread(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    while (1) {
        char *path = NULL;
        int ret = pthread_mutex_lock(&data->mutex);
        if (ret != 0) {
            fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }

        while (data->queue_head == NULL && !data->done) {
            ret = pthread_cond_wait(&data->cond, &data->mutex);
            if (ret != 0) {
                fprintf(stderr, "pthread_cond_wait failed: %s\n", strerror(ret));
                exit(EXIT_FAILURE);
            }
        }

        if (data->queue_head == NULL && data->done) {
            ret = pthread_mutex_unlock(&data->mutex);
            if (ret != 0) {
                fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
                exit(EXIT_FAILURE);
            }
            pthread_exit(NULL);
        }

        path = dequeue(data);
        ret = pthread_mutex_unlock(&data->mutex);
        if (ret != 0) {
            fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }

        if (path) {
            process_path(path, data);
            free(path);
        }
    }
}

/* -------------------------- Internal functions -------------------------- */

/**
 * @brief Processes a given file or directory path.
 *
 * This function processes the provided path, determining whether it is a file or directory.
 * Based on its type, it delegates to the appropriate handling function (file or directory).
 * 
 * @param path The file or directory path to process.
 * @param data Pointer to the shared ThreadData structure.
 */
static void process_path(char *path, struct ThreadData *data) {
    struct stat st;
    int ret;
    int error_in_this_call = 0;

    ret = lstat(path, &st);
    if (ret != 0) {
        fprintf(stderr, "du: cannot access '%s': %s\n", path, strerror(errno));
        error_in_this_call = 1;
    } else {
        handle_file(&st, data);
        if (S_ISDIR(st.st_mode)) {
            handle_directory(path, data);
        }
    }

    update_error_status(data, error_in_this_call);
}

/**
 * @brief Handles the processing of a regular file.
 *
 * This function processes a regular file, adding its size to the total size in the ThreadData structure.
 * 
 * @param st The stat structure containing file information.
 * @param data Pointer to the shared ThreadData structure.
 */
static void handle_file(const struct stat *st, struct ThreadData *data) {
    off_t size = st->st_blocks * 512;

    int ret = pthread_mutex_lock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
    data->total_size += size;
    ret = pthread_mutex_unlock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Handles the processing of a directory.
 *
 * This function processes a directory by traversing its contents and processing each file or
 * subdirectory within it. It updates the shared data with the directory's size.
 * 
 * @param path The directory path.
 * @param data Pointer to the shared ThreadData structure.
 */
static void handle_directory(const char *path, struct ThreadData *data) {
    DIR *dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "du: cannot read directory '%s': %s\n", path, strerror(errno));
        int ret = pthread_mutex_lock(&data->mutex);
        if (ret != 0) {
            fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }
        data->error_occurred = 1;
        ret = pthread_mutex_unlock(&data->mutex);
        if (ret != 0) {
            fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }
        return;
    }

    struct dirent *entry;
    int pending_increment = 0;
    size_t paths_capacity = 0;
    size_t paths_count = 0;
    char **paths_to_enqueue = NULL;

    while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
    }

    char *new_path = construct_path(path, entry->d_name);
    if (new_path == NULL) {
        perror("malloc");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    if (paths_count == paths_capacity) {
        if (paths_capacity == 0) {
            paths_capacity = 16;
        } else {
            paths_capacity *= 2;
        }
        char **tmp = realloc(paths_to_enqueue, paths_capacity * sizeof(char *));
        if (tmp == NULL) {
            perror("realloc");
            free(new_path);
            for (size_t i = 0; i < paths_count; i++) {
                free(paths_to_enqueue[i]);
            }
            free(paths_to_enqueue);
            closedir(dir);
            exit(EXIT_FAILURE);
        }
        paths_to_enqueue = tmp;
    }
    paths_to_enqueue[paths_count++] = new_path;
    pending_increment++;
    }

    int ret = pthread_mutex_lock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < paths_count; i++) {
        if (enqueue(data, paths_to_enqueue[i]) != 0) {
            perror("enqueue");
            pthread_mutex_unlock(&data->mutex);
            closedir(dir);
            exit(EXIT_FAILURE);
        }
    }
    data->pending += pending_increment;
    ret = pthread_cond_broadcast(&data->cond);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_broadcast failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
    ret = pthread_mutex_unlock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    free(paths_to_enqueue);

    if (closedir(dir) != 0) {
        fprintf(stderr, "closedir failed: %s\n", strerror(errno));
    }
}


/**
 * @brief Constructs a full file path from a directory and an entry name.
 *
 * This function creates a full path string by combining the directory path with the entry name.
 * 
 * @param dir_path The directory path.
 * @param entry_name The name of the directory entry (file or subdirectory).
 * @return A dynamically allocated string containing the full path.
 */
static char *construct_path(const char *dir_path, const char *entry_name) {
    size_t path_len = strlen(dir_path);
    size_t name_len = strlen(entry_name);
    size_t new_path_len = path_len + 1 + name_len + 1; // '/' and '\0'

    char *new_path = malloc(new_path_len);
    if (!new_path) {
        return NULL;
    }

    snprintf(new_path, new_path_len, "%s/%s", dir_path, entry_name);
    return new_path;
}

/**
 * @brief Updates the error status in the shared ThreadData structure.
 *
 * This function sets the error status flag in the shared data if an error occurs during file or directory processing.
 * 
 * @param data Pointer to the shared ThreadData structure.
 * @param error_in_this_call Error code for the current call.
 */
static void update_error_status(struct ThreadData *data, int error_in_this_call) {
    int ret = pthread_mutex_lock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }

    data->pending--;

    if (error_in_this_call) {
        data->error_occurred = 1;
    }

    if (data->pending == 0 && data->queue_head == NULL) {
        data->done = 1;
        ret = pthread_cond_broadcast(&data->cond);
        if (ret != 0) {
            fprintf(stderr, "pthread_cond_broadcast failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }
    }

    ret = pthread_mutex_unlock(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
}