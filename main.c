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
#define MAX_COMMANDS 10

//struct for array with split strings
typedef struct {
    char **args;
    char *command;

    char *redirectSymbol;
    char *writeFile;
    bool isRedirect;
    int cmd_id;
} Input;

//free char arrays
void free_array(char **arr) {
    for(int i = 0; i < MAX_STRINGS && arr[i] != NULL; i++)
    free(arr[i]);
}

//check if user entered a valid command
bool check_cmd(char *cmd){
    const char *COMMANDS[] = {"ls", "cd", "escape", "clear", "cat", "echo", "mv", "touch", "mkdir", "grep", NULL};
    bool known_cmd = false;
    //int length = (sizeof(COMMANDS) / sizeof(COMMANDS[0]));

    for(int i=0; COMMANDS[i] != NULL; i++) {

        if(strcmp(cmd, COMMANDS[i]) == 0) {
            known_cmd = true;
            break;
        }

    }

    return known_cmd;
}


char *trim(char *str) {
    // Trim leading whitespace
    while (isspace((unsigned char) *str)) str++;

    // If all spaces, return empty string
    if (*str == 0)
        return str;

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;
    *(end + 1) = '\0';

    return str;
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


void exec_pipes(Input** commands) {
    //execute command
    //write output (stdout) to stdin
    //child process reads stdin
    //child process adds this to args
    //repeat

    for(int i = 0; commands[i] != NULL; i++) {
        
    }

}


//process the input string - returns Input struct with split strings
Input** process_input(char inputStr[]) {


    Input** commands;
    commands = malloc(sizeof(Input)*MAX_COMMANDS);
    //const char s[4] = "|";

    //char* cmd_token = strtok(inputStr, s);
    char* current_command = malloc(strlen(inputStr));
    strcpy(current_command, inputStr);

    //holds individual strings separated by whitespace 
    char* strs[MAX_STRINGS];

    //tokenize the input by splitting by whitespace
    //copy individual inputs into an array of strings
    char* token = strtok(current_command, " ");

    int i = 0;
    while(token != NULL && i < MAX_STRINGS) {

        strs[i] = malloc(strlen(token) + 1);
        if (strs[i] == NULL) {
            perror("failed to allocate memory");
            exit(1);
        }
        strcpy(strs[i], token);
        printf("%s%s", strs[i], "\n");
        token = strtok(NULL, " ");
        i += 1;
    }

    //set null terminator
    strs[i] = NULL;

    int cmd_pos = 0;
    bool pipe = true;
    int curr_str = 0;

    //loop if pipes | are in input
    while(pipe) {
        //define input type for holding input values (terrible name)
        Input* vals;
        //printf("%s%s%c", "CURRENT COMMAND: ", cmd_token, '\n');
        pipe = false;
        vals = malloc(sizeof(Input));
        //allocate for input members
        vals->command = malloc(MAX_CMD_LEN * sizeof(char *));
        vals->args = malloc(MAX_STRINGS * sizeof(char *));
        vals->isRedirect = false;
        vals->cmd_id = cmd_pos;

        //check if there is a command or if it exists then copy into input member
        if(strs[curr_str] != NULL && check_cmd(strs[curr_str])) {
            strcpy(vals->command, strs[curr_str]);
        }

        else {
            printf("%c%s%c%s", '"', strs[curr_str], '"' ," is an unknown or missing command");
        }
            
        //current_command = current_command + cmd_pos;


        if(strs[curr_str + 1] != NULL) {
        //S int num_args = (sizeof(vals.strs) / sizeof(vals.strs[1])) - 1;

            //handle rest of the inputs (redirect symbols/args)
            for(int j = curr_str+1; strs[j] != NULL; j++) {
                printf("%s%s", "\nCURRENT INPUT: ", strs[j]);

                //handle redirection (configure redirect type and write file)
                if((strs[j][0] == '>' || strs[j][0] == '<') || vals->isRedirect) {

                    if(vals->isRedirect) {
                        //TODO: handle missing file
                        vals->writeFile = malloc(strlen(strs[j]));
                        strcpy(vals->writeFile, strs[j]);
                        continue;
                    }
                    vals->redirectSymbol = malloc(strlen(strs[j]));
                    strcpy(vals->redirectSymbol, strs[j]);
                    vals->isRedirect = true;

                }

                //set pipe
                else if (strcmp(strs[j], "|") == 0) {
                    printf("I'm triggered !!! \n");
                    pipe = true;
                    curr_str += 2;

                    break;
                }
                //else add to args
                else {
                    vals->args[j-1] = malloc(strlen(strs[j]) + 1);
                    strcpy(vals->args[j-1], strs[j]);
                }
            }

            curr_str++;


        }

        commands[cmd_pos] = vals;
        printf("%s%s%c", "\nINNER COMMAND: ", commands[cmd_pos]->command, '\n');
        cmd_pos++;
        //cmd_token = strtok(NULL, s);
        //free(vals);
        //vals = NULL;
        // free(current_command);
        // current_command = NULL;

    }

    commands[cmd_pos] = NULL;
    free_array(strs);
    return commands;
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

        Input** commands = process_input(inputStr);
        for(int i = 0; commands[i] != NULL; i++) {
            printf("%s%s%c", "COMMAND: ", commands[i]->command, '\n');
        } 

        Input results = *commands[0];

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