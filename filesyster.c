#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 
#include "./filesyster.h"


// Pipe file descriptors
static int pipe_to_fs[2] = {-1, -1};    // Interface → Filesystem
static int pipe_from_fs[2] = {-1, -1};  // Filesystem → Interface
static pid_t fs_pid = -1;

//check if the file descs are open
static int are_pipes_open() {
    return pipe_to_fs[0] != -1 && pipe_to_fs[1] != -1 &&
           pipe_from_fs[0] != -1 && pipe_from_fs[1] != -1;
}

void setup_pipes() {
    //check if the pipes are already open, else don't set!
    if (are_pipes_open()) {
        fprintf(stderr, "Pipes are already open.\n");
        return;
    }

    //open the pipes
    if((pipe(pipe_to_fs) == -1) || (pipe(pipe_from_fs) == -1)) {
        perror("failed to open pipes to filesyster app");
        exit(1);
    }

    fs_pid = fork();

    if(fs_pid == -1) {
        perror("failed to fork filesyster app");
    }

    if(fs_pid == 0) {

        if(dup2(pipe_to_fs[1], STDIN_FILENO) == -1 || dup2(pipe_from_fs[0], STDOUT_FILENO) == -1) {
            perror("dup2 failed for read/writing to filesyster app");
            exit(1);
        }

        close(pipe_to_fs[0]);
        close(pipe_from_fs[1]);

        execl(FILE_SYSTER_PATH, FILE_SYSTER_PATH, NULL);
    }

    else {
        // Parent process (Interface app)
        close(pipe_to_fs[0]);    // Close unused read end
        close(pipe_from_fs[1]);  // Close unused write end
    }

}

void send_to_filesystem(const char* input) {
    if (!are_pipes_open()) {
        fprintf(stderr, "Pipes are not open. Call setup_pipes() first.\n");
        return;
    }

    write(pipe_to_fs[1], input, strlen(input));
}


void read_from_filesystem(char* output, size_t size) {
    if (!are_pipes_open()) {
        fprintf(stderr, "Pipes are not open. Call setup_pipes() first.\n");
        return;
    }

    int bytes_read = read(pipe_from_fs[0], output, size - 1);
    if (bytes_read > 0) {
        output[bytes_read] = '\0';  // Null-terminate the string
    } else {
        output[0] = '\0';  // No data read
    }
}

// Cleanup pipes and terminate the filesystem app
void cleanup_pipes() {
    if (!are_pipes_open()) {
        fprintf(stderr, "Pipes are not open.\n");
        return;
    }

    // Close the pipes
    close(pipe_to_fs[1]);
    close(pipe_from_fs[0]);

    // Wait for the filesystem process to terminate
    if (fs_pid > 0) {
        waitpid(fs_pid, NULL, 0);
    }

    // Reset pipe and process state
    pipe_to_fs[0] = pipe_to_fs[1] = -1;
    pipe_from_fs[0] = pipe_from_fs[1] = -1;
    fs_pid = -1;
}
