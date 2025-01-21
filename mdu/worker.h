/**
 * @file worker.h
 * @brief Declaration for the worker thread function.
 *
 * This header file declares the `worker_thread` function, which is responsible
 * for processing file paths and calculating disk usage in parallel with other threads.
 * 
 * @author Emil Engvall
 * @date 21-10-2024
 */

#ifndef WORKER_H
#define WORKER_H

#include "thread.h"

/**
 * @brief Worker thread function.
 *
 * This function is executed by each worker thread. It processes file paths, handles
 * files and directories, and updates the shared thread data structure. The function
 * continues until all paths have been processed.
 * 
 * @param arg Pointer to the shared `ThreadData` structure.
 * @return NULL when the thread finishes execution.
 */
void *worker_thread(void *arg);

#endif // WORKER_H
