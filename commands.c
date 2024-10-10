#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include "./commands.h"
#include <unistd.h>
#define MY_STDOUT STDOUT_FILENO

//ESCAPE
void escape() {
    printf("%s", "Goodbye =)");
    exit(0);
}

//CHANGE DIRECTORY
void cd(char **args) {
    char *path;
    if(args[0] == NULL) {
        path = ("/home/isaakpi");
    } 
    else {
        path = args[0];
    }
    printf("Changed directory to: %s\n", path);  

    if(chdir(path) != 0) {
        perror("cd failed");
    }
    else {
        printf("Changed directory to: %s\n", path);  
    }

}

//CLEAR
void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
}

 void run_commands(char* cmd, char** args) {
    char path[100] = "/home/isaakpi/Desktop/c_projects/terminal_project/";
    int args_count = 0;

    //calculate number of args
    while (args[args_count] != NULL) {
        args_count++;
    }

    //size of new arg list
    char *argsv[args_count + 2];
    for(int i = 0; i < args_count + 2;i++) {
        argsv[i] = NULL;
    }

    //set path file 
    strcat(path, cmd);
    argsv[0] = strdup(path);

    //dupe args to new arg list
    for (int i = 1; args[i-1] != NULL; i++) {
        argsv[i] = strdup(args[i-1]);
    }


    //set null terminator
    args[args_count+1] = NULL;

    //FOR DEBUGGING
    // for (int i = 0; argsv[i] != NULL; i++) {
    //     printf("argsv[%d]: %s\n", i, argsv[i]);
    // }


     if(strcmp(cmd, "ls") == 0) {
        execvp(argsv[0], argsv);
    }

     else if(strcmp(cmd, "cat") == 0) {
        execvp(argsv[0], argsv);
    }
     else if(strcmp(cmd, "echo") == 0) {
        execvp(argsv[0], argsv);
    }

    for(int i=0; i< args_count + 1; i++) {
        free(argsv[i]);
    }

}

bool run_local_commands(char* cmd, char** args) {

    if(strcmp(cmd, "escape") == 0) {
        escape();
        return true;
    }
    else if(strcmp(cmd, "cd") == 0) {
        cd(args);
        return true;
    }

    else if(strcmp(cmd, "clear") == 0) {
        clearScreen();
        return true;
    }


    return false;
}

