#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "quiz.h"

#define CURL_PATH "/usr/bin/curl"

// Helper function to read dat from a file descriptor
static char* read_response(int fd) {
    char *response = NULL;
    size_t length = 0;
    char buffer[256];
    ssize_t bytes_read;

    // Readng chunks of data into buffer until the end
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        response = realloc(response, length + bytes_read + 1);
        if (response == NULL) {
            printf("Memory Allocation Failed\n");
            exit(EXIT_FAILURE);
        }
        memcpy(response + length, buffer, bytes_read);
        length += bytes_read;
    }
    if (response) {
        response[length] = '\0';  // Null-terminate string
    }
    return response;
}

// Fetch function to retrieve data from URL usng curl as a child process
char* fetch(char *url) {
    int pipe_fds[2];
    pid_t pid;
    char *cmd[] = { CURL_PATH, "-s", url, NULL };
    char *result = NULL;

    // Creating a pipe
    if (pipe(pipe_fds) == -1) {
        perror("pipe error");
        return NULL;
    }

    // Forking a new process
    pid = fork();
    if (pid == -1) {
        perror("fork error");
        return NULL;
    } else if (pid == 0) { // Child process
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
        execv(CURL_PATH, cmd);
        perror("execv faled");
        exit(EXIT_FAILURE);
    } else { // Parent proces
        close(pipe_fds[1]);
        result = read_response(pipe_fds[0]);
        close(pipe_fds[0]);
    }

    return result;
}
