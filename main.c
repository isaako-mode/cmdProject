#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 
#include "./filesyster.h"


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
    bool isPipe;
} Input;

//ESCAPE function for when user exits
void escape() {
    printf("%s", "Goodbye =)\n");
    exit(0);
}

//checks for prefix
bool StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

//check if command is local for dealing with pipes
bool is_local_cmd(char* cmd) {
    const char* LOCAL_CMDS[] = {"cd", "escape", "clear", NULL};
    bool is_local = false;

    for(int i=0; LOCAL_CMDS[i] != NULL; i++) {

        if(strcmp(cmd, LOCAL_CMDS[i]) == 0) {
            is_local = true;
            break;
        }

    }

    return is_local;
}

//free char arrays
void free_array(char **arr) {
    for(int i = 0; i < MAX_STRINGS && arr[i] != NULL; i++)
    free(arr[i]);
}

//check if user entered a valid command
bool check_cmd(char *cmd){
    const char* COMMANDS[] = {"ls", "cd", "escape", "clear", "cat", "echo", "mv", "touch", "mkdir", "grep", NULL};
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

//Function for handling pipes. The input is an array of process commands 
void exec_pipes(Input** commands) {
    //execute command
    //write output (stdout) to stdin
    //child process reads stdin
    //child process adds this to args
    //repeat

    int i = 0;
    int prev_fd = -1;
    //setup pipe file descriptors
    int pipe_fds[2];
    while(commands[i] != NULL) {
        Input* curr_command = commands[i];

        if (commands[i + 1] != NULL) {  // Check if there is a next command to pipe to
            if (pipe(pipe_fds) == -1) {
                perror("Pipe creation failed");
                exit(1);
            }
        }

        //check if local command and handle accordingly
        if(is_local_cmd(curr_command)) {

            if(prev_fd != -1) {  // Redirect stdin from the previous pipe
                dup2(prev_fd, STDIN_FILENO);

                if(close(prev_fd) == -1) {
                    perror("Error closing prev_fd");
                    exit(1);
                }

                //prev_fd = -1;
            }

            if(commands[i + 1] != NULL) {  // Redirect stdout to the current pipe
                dup2(pipe_fds[1], STDOUT_FILENO);

                if(close(pipe_fds[1]) == -1) {
                    perror("Error closing pipe_fds[1]");
                    exit(1);
                }
            }

            run_local_commands(curr_command->command, curr_command->args);
            //fflush(stdout);

            // if(commands[i + 1] != NULL) { 
            //     if (close(pipe_fds[0]) == -1) {
            //     perror("Error closing pipe_fds[0]");
            //     exit(1);
            //     }
            // }
            
        }

        else {
            pid_t pid = fork();

            //if fork failed
            if(pid == -1) {
                printf("failed to fork");
                exit(1);
            }

            //if we are the parent process, wait for child to terminate
            else if(pid > 0) {

                if(prev_fd != -1) {
                    if (close(prev_fd) == -1) {
                        perror("Error closing prev_fd");
                        exit(1);
                    }
                }

                if(commands[i + 1] != NULL) {
                    if(close(pipe_fds[1]) == -1) {
                        perror("Error closing pipe_fds[1]");
                        exit(1);
                    }

                    prev_fd = pipe_fds[0];
                }

                waitpid(pid, NULL, 0);
                i++;
            }

            //if we are a child process, handle external commands 
            else {

                // Redirect stdin to the previous pipe if it's not the first command
                if(prev_fd != -1) {
                    if(dup2(prev_fd, STDIN_FILENO) == -1) {
                        perror("Dup2 failed for STDIN_FILENO");
                        exit(1);
                    }

                    if(close(prev_fd) == -1) {
                        perror("Error child closing prev fd");
                        exit(1);
                    }
                }

                // Redirect stdout to the current pipe if there is a next command
                if(commands[i + 1] != NULL) {
                    if(dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
                        perror("dup2 failed");
                    }

                    if(close(pipe_fds[1]) == -1) {
                        perror("Error closing pipe_fds[1]");
                        exit(1);
                    }
                }

                //Close unused file descriptors
                if(commands[i + 1] != NULL) {
                    if(close(pipe_fds[0]) == -1) {
                        perror("Error closing pipe_fds[0]");
                        exit(1);
                    }

                }

                // Run external command
                run_commands(curr_command->command, curr_command->args);
                exit(0);  // Exit after executing the command
            }

        }
    }

    // if (prev_fd != -1) {
    //     close(prev_fd);
    // }
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
        token = strtok(NULL, " ");
        i += 1;
    }

    //set null terminator
    strs[i] = NULL;

    int cmd_pos = 0;
    bool pipe = true;
    int curr_str = 0;
    int arg_pos = 0;

    //loop if pipes | are in input
    while(pipe) {
        //define input type for holding input values (terrible name)
        Input* vals;

        pipe = false;
        vals = malloc(sizeof(Input));
        //allocate for input members
        vals->command = malloc(MAX_CMD_LEN * sizeof(char *));
        vals->args = malloc(MAX_STRINGS * sizeof(char *));
        vals->isRedirect = false;
        vals->cmd_id = cmd_pos;
        vals->isPipe = false;

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
                    vals->isPipe = true;
                    pipe = true;

                    //check if there were arguments for previous function
                    //and increment next string accordingly (past the pipe if args)
                    if(vals->args[0] == NULL) {curr_str++;}
                    else {curr_str += 2;}

                    break;
                }
                //else add to args
                else {
                    vals->args[arg_pos] = malloc(strlen(strs[j]) + 1);
                    strcpy(vals->args[arg_pos], strs[j]);
                    arg_pos++;
                }
            }

            curr_str++;
            arg_pos = 0;


        }

        commands[cmd_pos] = vals;
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
    setup_pipes();
    //loop for terminal
    while(1==1) {
        printf("\nEnter a command or type 'escape' to exit $ ");

        //get user input
        fgets(inputStr, sizeof(inputStr), stdin);

        //set ending newline to null terminator
        inputStr[strcspn(inputStr, "\n")] = '\0';

        if(strcmp(inputStr, "escape") == 0) {
            escape();
        }

        if(StartsWith(inputStr, "fs")) {
            char output[256];

            send_to_filesystem(inputStr);

            // Read output from the filesystem app
            read_from_filesystem(output, sizeof(output));
            printf("Filesystem Output: %s\n", output);
        }

        //Parses input and creates input structs (double pointer to handle pipes)
        Input** commands = process_input(inputStr);

        //handle input with pipes
        if(commands[0]->isPipe) {
            exec_pipes(commands);
            continue;
        } 

        //TODO: need to add condition to exec single command vs pipeline... Maybe add flag to indicate whether or not there is a pipe in input?
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