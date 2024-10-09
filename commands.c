#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include "./commands.h"
#include <unistd.h>

//ECHO
void echo(char** args) {
    if(args[0] != NULL) {
        for(int i=0; args[i] != NULL; i++) {
            printf("%s%c", args[i], ' ');
        }
    }
    else {
        printf("Nothing to print!");
    }
}

//CAT
void cat(char** args) {
    FILE *fptr;

    if(args[0] != NULL) {

        fptr = fopen(args[0], "r");
    }

    char text[500];

    if(fptr != NULL){

        while(fgets(text, 500, fptr)) {
            printf("%s", text);
        }
    }
    else {
        printf("no file foo");
    }
}

//LS
void list(char** args){
    //Define directory struct
    struct dirent *de;
    DIR *dr;

    if(args[0] == NULL) {
        dr = opendir(".");
    } 
    else if(strcmp(args[0], "~") == 0) {
        dr = opendir("/home/isaakpi");
    }

    else {
        dr = opendir(args[0]);
    }

    if (dr == NULL) {
        printf("Could not open current directory" ); 
        return;
    }

    char hidden = '.';
    while ((de = readdir(dr)) != NULL) {
        //SEG FAULT HERE!!
        if(strncmp(de->d_name, &hidden, 1) == 0) {
            continue;
        }

        printf("%s  ", de->d_name); 
    }
    printf("%s  ", "PORN");
  
    closedir(dr);     
    return; 
}

//ESCAPE
void escape() {
    printf("%s", "Goodbye =)");
    exit(0);
}

//CHANGE DIRECTORY
void cd(char **args) {
    const char *path;
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

    list(args);
}

//CLEAR
void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
}

char* run_commands(char* cmd, char** args) {

     if(strcmp(cmd, "escape") == 0) {
        escape();
    }
     else if(strcmp(cmd, "ls") == 0) {
        list(args);
    }
     else if(strcmp(cmd, "clear") == 0) {
        clearScreen();
    }
     else if(strcmp(cmd, "cd") == 0) {
        cd(args);
    }
     else if(strcmp(cmd, "cat") == 0) {
        cat(args);
    }
     else if(strcmp(cmd, "echo") == 0) {
        echo(args);
    }
}

