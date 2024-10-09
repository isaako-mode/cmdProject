#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>

#define MAX_STRINGS 50
#define MAX_CMD_LEN 15

//struct for array with split strings
typedef struct {
    char **strs;
    char *command;
    char **args;
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

//process the input string - returns Input struct with split strings
Input process_input(char inputStr[]) {
    char *token = strtok(inputStr, " ");
    Input vals;
    vals.strs = malloc(MAX_STRINGS * sizeof(char *));
    vals.command = malloc(MAX_CMD_LEN * sizeof(char *));
    vals.args = malloc(MAX_STRINGS * sizeof(char *));

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

    vals.strs[i] = NULL;

    if(vals.strs[0] != NULL && check_cmd(vals.strs[0])) {
        strcpy(vals.command, vals.strs[0]);
    }

    else {
        printf("%c%s%c%s", '"', vals.strs[0], '"' ," is an unknown or missing command");
    }

    if(vals.strs[1] != NULL) {
       //S int num_args = (sizeof(vals.strs) / sizeof(vals.strs[1])) - 1;

        for(int j = 1; vals.strs[j] != NULL; j++) {
            vals.args[j-1] = malloc(strlen(vals.strs[j]) + 1);
            strcpy(vals.args[j-1], vals.strs[j]);
        }
    }

    return vals;
    
}

int main() {
    //user input 
    char inputStr[100];

    //loop for terminal
    while(1==1) {

        printf("\nEnter a command or type 'escape' to exit $ ");

        //get user input
        fgets(inputStr, sizeof(inputStr), stdin);

        //set ending newline to null terminator
        inputStr[strcspn(inputStr, "\n")] = '\0';

        //check if user types in "exit"
        // if (strcmp(inputStr, "escape") == 0){
        //     exit(0);
        // }

        Input results = process_input(inputStr);

        //printf("%s\n", results.command);

        run_commands(results.command, results.args);

        free_array(results.strs);
        free_array(results.args);
        free(results.command);
        memset(inputStr, '\0', sizeof(inputStr));
    }
    return 0;
}


//read input
//process input
// -split the string by spaces
// -extract command and inputs
// -execute command