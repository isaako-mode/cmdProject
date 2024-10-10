#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 


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
} Input;

//free char arrays
void free_array(char **arr) {
    for(int i = 0; i < MAX_STRINGS && arr[i] != NULL; i++)
    free(arr[i]);
}

//check if user entered a valid command
bool check_cmd(char *cmd){
    const char *COMMANDS[] = {"ls", "cd", "escape", "clear", "cat", "echo", NULL};
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
        printf("dup2 failed");
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
    if(vals.strs[0] != NULL && check_cmd(vals.strs[0])) {
        strcpy(vals.command, vals.strs[0]);
    }

    else {
        printf("%c%s%c%s", '"', vals.strs[0], '"' ," is an unknown or missing command");
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

    //save stdout file desc
    const int saveStdout = dup(STDOUT_FILENO);

    if(saveStdout == -1) {
        printf("dup failed");
        exit(1);
    }
    //loop for terminal
    while(1==1) {
        printf("\nEnter a command or type 'escape' to exit $ ");

        //get user input
        fgets(inputStr, sizeof(inputStr), stdin);

        //set ending newline to null terminator
        inputStr[strcspn(inputStr, "\n")] = '\0';

        Input results = process_input(inputStr);

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
                waitpid(pid, NULL, 0);
            }

            //if we are a child process, handle external commands 
            else {
                //check redirect flag and configure redirect if needed
                if(results.isRedirect) {
                    fflush(stdout);
                    configure_redirection(results.redirectSymbol, results.writeFile);
            }

                //run external commands as a child process
                run_commands(results.command, results.args);

                if(dup2(saveStdout, STDERR_FILENO) == -1) {
                    printf("dup2 failed");
                    exit(1);
                }

                exit(0);
        }

        }

        // if(results.redirectSymbol != NULL){
        //     free(results.redirectSymbol);
        // }

        //free(results.writeFile);
        free_array(results.strs);
        free_array(results.args);
        free(results.command);
        memset(inputStr, '\0', sizeof(inputStr));
        results.isRedirect = false;

    }

    return 0;

}


//read input
//process input
// -split the string by spaces
// -extract command and inputs
// -execute command