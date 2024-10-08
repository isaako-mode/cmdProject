#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_STRINGS 50
#define MAX_CMD_LEN 15
#define COMMANDS ["ls", "cd"]

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
    bool known_cmd = false;
    int length = (sizeof(COMMANDS) / sizeof(COMMANDS[0]));

    for(i=0; i < length; i++) {
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
    
    return vals;
    
}

int main() {
    char inputStr[100];

    while(1==1) {

        printf("Enter a command or type 'escape' to exit\n");

        //get user input
        fgets(inputStr, sizeof(inputStr), stdin);

        inputStr[strcspn(inputStr, "\n")] = '\0';

        if (strcmp(inputStr, "escape") == 0){
            exit(0);
        }

        Input results = process_input(inputStr);
        printf("%s\n", results.strs[0]);
        free_array(results.strs);
        memset(inputStr, '\0', sizeof(inputStr));
    }
    return 0;
}


//read input
//process input
// -split the string by spaces
// -extract command and inputs
// -execute command