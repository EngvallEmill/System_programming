#ifndef COMMAND_H
#define COMMAND_H

/**
 * @file command.h
 * @brief Declarations for executing commands.
 * 
 * This module provides functions to execute commands, handle 
 * input/output redirection, and manage processes.
 * 
 * Error handling: Errors are managed by printing messages with perror and exiting 
 * on critical failures, such as errors in fork, dup2, or exec.
 * 
 * Memory management: The memory for commands needs to be manually freed by calling free_commands.
 * 
 * @date 14-10-2024
 * @author Emil Engvall
 */

#include <stdio.h>

/**
 * @brief Executes all commands by creating processes and setting up pipes.
 * 
 * This function manages the execution flow by forking processes, setting up redirection,
 * and waiting for all child processes to finish.
 * 
 * @param commands Array of commands to execute.
 * @param cmd_count The number of commands.
 * @param pipes Array of pipes for connecting commands.
 * @return Exit status for the entire process.
 */
int execute_commands(char ***commands, int cmd_count, int **pipes);

/**
 * @brief Forks and executes a single command.
 * 
 * This function forks a new process for the command at index i, sets up redirection,
 * and executes the command. Stores the child's PID in the pids array.
 * 
 * @param i The index of the current command.
 * @param commands The array of commands.
 * @param cmd_count The total number of commands.
 * @param pipes The array of pipes.
 * @param pids The array to store child process IDs.
 */
void fork_and_execute_command(int i, char ***commands, int cmd_count, int **pipes, int *pids);

/**
 * @brief Waits for all child processes to finish.
 * 
 * This function waits for all child processes whose PIDs are stored in the pids array,
 * and updates the exit_status accordingly.
 * 
 * @param pids The array of child process IDs.
 * @param cmd_count The total number of commands.
 * @param exit_status Pointer to the exit status variable to update.
 */
void wait_for_children(int *pids, int cmd_count, int *exit_status);

/**
 * @brief Sets up input/output redirection for a command.
 * 
 * Redirects the standard input and output of the current process to the appropriate pipes.
 * 
 * @param i The index of the current command.
 * @param cmd_count The total number of commands.
 * @param pipes The array of pipes.
 */
void setup_redirection(int i, int cmd_count, int **pipes);

/**
 * @brief Closes all pipes.
 * 
 * Closes all file descriptors associated with the pipes.
 * 
 * @param pipes The array of pipes.
 * @param cmd_count The total number of commands.
 */
void close_all_pipes(int **pipes, int cmd_count);

#endif // COMMAND_H
