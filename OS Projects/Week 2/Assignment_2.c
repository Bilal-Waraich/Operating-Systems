/*
Operating Systems
Assignment 2
Bilal Waraich
bwaraich@constructor.university
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>

#define MAX_ARGS 1024  // Maximum number of arguments in the command array

// Function to run a command using execvp
void execute_command(char *cmd[], int display_cmd) {
    pid_t pid = fork();  // Creating a child process
    
    if (pid == -1) {  // Error in fork
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // Child process
        if (display_cmd) {  // If -t is enabled, show the command
            for (int i = 0; cmd[i] != NULL; i++) {
                fprintf(stderr, "%s ", cmd[i]);
            }
            fprintf(stderr, "\n");
        }
        execvp(cmd[0], cmd);  // Executing the command using execvp
        perror("exec failed");  // If execvp fails
        exit(EXIT_FAILURE);
    }
}

// Function to wait for jobs if too many processes are running
void wait_for_jobs(int *active_jobs) {
    while (*active_jobs > 0) {
        wait(NULL);  // Wait for any child process to finish
        (*active_jobs)--;  // Decrease the count of active jobs
    }
}

int main(int argc, char *argv[]) {
    // Variables to store options and command-line arguments
    int max_args = MAX_ARGS, show_cmd = 0, max_jobs = 1, active_jobs = 0;
    int option;
    char *command = "/bin/echo";  // Default command if none provided

    // Parsing command-line options using getopt
    while ((option = getopt(argc, argv, "n:tj:")) != -1) {
        switch (option) {
            case 'n':  // Option to limit the number of arguments per command
                max_args = atoi(optarg) + 1;  // Convert string to integer, adding 1 for the command itself
                break;
            case 't':  // Option to display the command before execution
                show_cmd = 1;  // Enable the flag to show the command
                break;
            case 'j':  // Option to limit the number of concurrent jobs
                max_jobs = atoi(optarg);  // Set the maximum number of concurrent jobs
                break;
            default:
                fprintf(stderr, "Usage: %s [-n num] [-t] [-j jobs] [command]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // If a command is provided in the arguments, use it instead of default /bin/echo
    if (optind < argc) {
        command = argv[optind];
    }

    // Array to hold the command and arguments
    char *cmd[MAX_ARGS];
    cmd[0] = command;  // Command is stored in cmd[0]
    int cmd_len = 1;   // Initialize length of the command array
    
    // Line-related variables
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // Reading input from stdin and constructing the argument list
    while ((nread = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\n")] = '\0';  // Remove newline character
        cmd[cmd_len++] = strdup(line);     // Add line to command arguments

        // If the command reaches max_args limit, execute it
        if (cmd_len == max_args) {
            cmd[cmd_len] = NULL;  // Null-terminate the command array
            execute_command(cmd, show_cmd);  // Execute the command
            active_jobs++;  // Increase the count of active jobs

            // If the number of active jobs reaches max_jobs, wait for them to finish
            if (active_jobs == max_jobs) {
                wait_for_jobs(&active_jobs);  // Wait for jobs to complete
            }

            cmd_len = 1;  // Reset cmd_len for the next set of arguments
        }
    }

    // Execute any remaining commands if they haven't been processed
    if (cmd_len > 1) {
        cmd[cmd_len] = NULL;  // Null-terminate the array
        execute_command(cmd, show_cmd);  // Execute the remaining command
        active_jobs++;  // Increase the count of active jobs
    }

    // Wait for all remaining jobs to finish before exiting
    wait_for_jobs(&active_jobs);

    // Free the allocated memory for the input line
    free(line);

    return 0;
}
