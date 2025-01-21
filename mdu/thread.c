#include "thread.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int thread_data_init(struct ThreadData *data) {
    memset(data, 0, sizeof(struct ThreadData));
    int ret = pthread_mutex_init(&data->mutex, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_init failed: %s\n", strerror(ret));
        return -1;
    }
    ret = pthread_cond_init(&data->cond, NULL);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_init failed: %s\n", strerror(ret));
        pthread_mutex_destroy(&data->mutex);
        return -1;
    }
    data->total_size = 0;
    data->done = 0;
    data->pending = 0;
    data->queue_head = NULL;
    data->queue_tail = NULL;
    data->error_occurred = 0;
    return 0;
}

void thread_data_destroy(struct ThreadData *data) {
    int ret = pthread_mutex_destroy(&data->mutex);
    if (ret != 0) {
        fprintf(stderr, "pthread_mutex_destroy failed: %s\n", strerror(ret));
    }
    ret = pthread_cond_destroy(&data->cond);
    if (ret != 0) {
        fprintf(stderr, "pthread_cond_destroy failed: %s\n", strerror(ret));
    }

    while (data->queue_head) {
        struct DirNode *tmp = data->queue_head;
        data->queue_head = data->queue_head->next;
        free(tmp->path);
        free(tmp);
    }
}

int enqueue(struct ThreadData *data, char *path) {
    struct DirNode *new_node = malloc(sizeof(struct DirNode));
    if (!new_node) {
        return -1;
    }
    new_node->path = path;
    new_node->next = NULL;
    if (data->queue_tail) {
        data->queue_tail->next = new_node;
        data->queue_tail = new_node;
    } else {
        data->queue_head = new_node;
        data->queue_tail = new_node;
    }
    return 0;
}

char *dequeue(struct ThreadData *data) {
    if (data->queue_head == NULL) {
        return NULL;
    }
    struct DirNode *node = data->queue_head;
    char *path = node->path;
    data->queue_head = node->next;
    if (data->queue_head == NULL) {
        data->queue_tail = NULL;
    }
    free(node);
    return path;
}