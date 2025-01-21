#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <sys/types.h>

struct DirNode {
    char *path;
    struct DirNode *next;
};

struct ThreadData {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct DirNode *queue_head;
    struct DirNode *queue_tail;
    off_t total_size;
    int pending;
    int done;
    int error_occurred;
};

int thread_data_init(struct ThreadData *data);
void thread_data_destroy(struct ThreadData *data);
int enqueue(struct ThreadData *data, char *path);
char *dequeue(struct ThreadData *data);

#endif // THREAD_H
