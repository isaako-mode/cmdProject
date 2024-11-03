#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

#define MAX_ARG_LEN
#define MAX_STRINGS 50
#define MAX_CMD_LEN 15

//struct for array with split strings
typedef struct {
    char **strs;
    char *command;
    char **args;

    char *redirectSymbol;
    char *writeFile;
    bool isRedirect;
    bool isPipe;
} Input;

//for adding a new argument if needed
char** add_args(char** initialArgs, char* newArg) {
    
    int arg_count = 0;
    while(initialArgs[arg_count] != NULL) {
        printf("%s", "curr arg: ");
        arg_count++;
    }

    char** newArgs = malloc((arg_count + 2)*sizeof(char*));
    newArgs[0] = strdup(newArg);


    if(newArgs == NULL) {
        perror("Malloc failed!");
        exit(1);
    }

    for(int i = 1; i < arg_count; i++) {
        newArgs[i] = initialArgs[i];
    }

    newArgs[arg_count + 1] = NULL;

    return newArgs;

}

//free results object's attributes
void free_results(Input *result) {
    free_array(result->strs);
    free_array(result->args);
    free(result->command);
    result->strs = NULL;
    result->args = NULL;
    result->command = NULL;
}

//free char arrays
void free_array(char **arr) {
    for(int i = 0; i < MAX_STRINGS && arr[i] != NULL; i++)
    free(arr[i]);
}

//check if user entered a valid command
bool check_cmd(char* cmd){
    const char* COMMANDS[] = {"ls", "cd", "escape", "clear", "cat", "echo", "mv", "touch", "mkdir", "grep", NULL};
    bool known_cmd = false;
    int length = (sizeof(COMMANDS) / sizeof(COMMANDS[0]));

    for(int i=0; COMMANDS[i] != NULL; i++) {

        if(strcmp(cmd, COMMANDS[i]) == 0) {
            known_cmd = true;
            break;
        }

    }

    return known_cmd;
}

//redirect stdout to write file
void configure_redirection(char *redirectSymbol, char *writeFile) {
    //file descriptor
    int fd;

    //check redirect types and set correct mode
    if(strcmp(redirectSymbol, ">>") == 0) {
        fd = open(writeFile, O_WRONLY | O_APPEND | O_CREAT, 0644);
    }

    else if(strcmp(redirectSymbol, ">") == 0) {
        fd = open(writeFile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    }

    //error handling
    if(dup2(fd, STDOUT_FILENO) == -1) {
        printf("dup2 failed at redirect config ");
    }

    close(fd);
}

//process the input string - returns Input struct with split strings
Input process_input(char inputStr[]) {

    //tokenize the input by splitting by whitespace
    char *token = strtok(inputStr, " ");

    //define input type for holding input values (terrible name)
    Input vals;

    //allocate for input members
    vals.strs = malloc(MAX_STRINGS * sizeof(char *));
    vals.command = malloc(MAX_CMD_LEN * sizeof(char *));
    vals.args = malloc(MAX_STRINGS * sizeof(char *));
    vals.isRedirect = false;
    vals.isPipe = false;


    //copy individual inputs into an array of strings
    int i = 0;
    while(token != NULL && i < MAX_STRINGS) {

        vals.strs[i] = malloc(strlen(token) + 1);
        if (vals.strs[i] == NULL) {
            perror("failed to allocate memory");
            exit(1);
        }
        strcpy(vals.strs[i], token);

        token = strtok(NULL, " ");
        i += 1;
    }

    //set null terminator
    vals.strs[i] = NULL;

    //check if there is a command or if it exists then copy into input member
    if(vals.strs[0] != NULL) {
        strcpy(vals.command, vals.strs[0]);
    }

    if(vals.strs[1] != NULL) {
       //S int num_args = (sizeof(vals.strs) / sizeof(vals.strs[1])) - 1;
        //handle rest of the inputs (redirect symbols/args)
        for(int j = 1; vals.strs[j] != NULL; j++) {

            //handle redirection (configure redirect type and write file)
            if((vals.strs[j][0] == '>' || vals.strs[j][0] == '<') || vals.isRedirect) {

                if(vals.isRedirect) {
                    //TODO: handle missing file
                    vals.writeFile = malloc(strlen(vals.strs[j]));
                    strcpy(vals.writeFile, vals.strs[j]);
                    continue;
                }
                vals.redirectSymbol = malloc(strlen(vals.strs[j]));
                strcpy(vals.redirectSymbol, vals.strs[j]);
                vals.isRedirect = true;

            }

            //set pipe
            if(vals.strs[j][0] == '|') {
                vals.isPipe = true;
                
                char* newInput = malloc(strlen(inputStr) - j + 1);
                if(newInput == NULL) {
                    perror("malloc failed");
                    exit(1);
                }

                strcpy(newInput, inputStr + j);
                //free(inputStr);

                inputStr = newInput;
                break;
            }

            //else add to args
            else {
                vals.args[j-1] = malloc(strlen(vals.strs[j]) + 1);
                strcpy(vals.args[j-1], vals.strs[j]);
            }
        }
    }

    return vals;
    
}

int main() {
    //user input 
    char inputStr[100];
    bool pipes = false;
    char* outputStr;
    int pipe_fds[2];
    int prev_fd = -1;
    char buffer[1024];

    //save stdout file desc
    const int saveStdout = dup(STDOUT_FILENO);

    if(saveStdout == -1) {
        printf("dup failed");
        exit(1);
    }
    //loop for terminal
    while(1==1) {

        if(!pipes) {
            printf("\nEnter a command or type 'escape' to exit $ ");

            //get user input
            fgets(inputStr, sizeof(inputStr), stdin);

            //set ending newline to null terminator
            inputStr[strcspn(inputStr, "\n")] = '\0';
        }

        Input results = process_input(inputStr);
        
        //check if command exists and clear inputs if it doesn't
        if (!check_cmd(results.command)) {
            printf("%c%s%c%s", '"', results.command, '"' ," is an unknown or missing command");
            free_results(&results);
            memset(inputStr, '\0', sizeof(inputStr));
            memset(outputStr, '\0', sizeof(outputStr));

            results.isRedirect = false;
            results.isPipe = false;
            continue;
        }
        //if input string contains a prepare to set file descriptor
        if(results.isPipe) {

            pipes = true;
            if(pipe(pipe_fds) == -1) {
                printf("Pipe fd failed!");
                exit(1);
            }

            if(prev_fd != -1) {
                char** temp = results.args;
                results.args = add_args(temp, outputStr);
            }

        }

        else {
            pipes = false;
        }

        //If not a local command, run create child and run external
        bool local = run_local_commands(results.command, results.args);
        if(!local) {
            
            pid_t pid = fork();

            //if fork failed
            if(pid == -1) {
                printf("failed to fork");
                exit(1);
            }

            //if we are the parent process, wait for child to terminate
            else if(pid > 0) {

                if(prev_fd != -1) {
                    close(prev_fd);
                }
                if(pipes) {
                    close(pipe_fds[1]);
                    prev_fd = pipe_fds[0];
                }
                else {
                    prev_fd = -1;
                }
  
            }

            //if we are a child process, handle external commands 
            else {
                //check redirect flag and configure redirect if needed
                if(results.isRedirect) {
                    fflush(stdout);
                    configure_redirection(results.redirectSymbol, results.writeFile);
            }
            //MOVE BACK TO LINE 286
            waitpid(pid, NULL, 0);

            if(pipes) {
                printf("HANDLING PIPE FOR OUTPUT\n");
                //fflush(stdout);

            //if there is a pipe, set file desc to the output 
                close(pipe_fds[0]);
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]);
            }

            //if not first commnad, read from the previous pipe
            if(prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            //run external commands as a child process
            run_commands(results.command, results.args);

            
            if(dup2(saveStdout, STDOUT_FILENO) == -1) {
                printf("dup2 failed when redirecting after pipes");
                exit(1);
            }

            //READ OUTPUT FOR PIPES
            if(pipes) {
                printf("SETTING OUTPUT\n");

                close(pipe_fds[1]);

                int nbytes = read(pipe_fds[0], buffer, sizeof(buffer) - 1);

                if(nbytes > 0) {
                    buffer[nbytes] = '\0';
                    outputStr = malloc(nbytes + 1);

                    if(outputStr == NULL) {
                        printf("FAILED TO ALLOCATE OUTPUTSTR\n");
                        exit(1);
                    }
                    strcpy(outputStr, buffer);
                    printf("%s%s""OUTPUT STRING: ", outputStr);
                }

                else {
                    printf("No data in Pipe or read failed!\n");
                }
                

            }

            close(pipe_fds[0]);
            }


        }

        // if(results.redirectSymbol != NULL){
        //     free(results.redirectSymbol);
        // }

        //free(results.writeFile);
        if(!pipes) {
            memset(inputStr, '\0', sizeof(inputStr));
        }
        free_results(&results);
        results.isRedirect = false;

    }

    return 0;

}


//read input
//process input
// -split the string by spaces
// -extract command and inputs
// -execute command