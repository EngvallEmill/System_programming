#ifndef PIPES_H
#define PIPES_H

/**
 * @file pipes.h
 * @brief Function declarations for managing pipes between commands.
 * 
 * This module provides functions for creating pipes for communication between 
 * commands, handling setup, and closing all file descriptors associated with the pipes.
 * 
 * Error handling: This module handles errors by printing error messages using `perror` 
 * and immediately exiting the program on critical failures such as memory allocation or pipe creation errors.
 * 
 * Memory management: You need to manually free the memory allocated for the pipes using `close_pipes`.
 * 
 * @date 14-10-2024
 * @author Emil Engvall
 */


/**
 * @brief Creates an array of pipes for inter-process communication.
 * 
 * This function allocates memory for and sets up the necessary number of pipes 
 * for communication between processes.
 * 
 * @param num_pipes The number of pipes to create.
 * @return A double pointer to the array of pipes.
 */
int **setup_pipes(int num_pipes);

/**
 * @brief Frees memory and closes all pipes.
 * 
 * Closes all file descriptors and deallocates memory associated with the pipes.
 * 
 * @param pipes The array of pipes to close.
 * @param num_pipes The number of pipes.
 */
void close_pipes(int **pipes, int num_pipes);

#endif // PIPES_H
/**
 * @}
 */