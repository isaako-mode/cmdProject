#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include "./commands.h"

void list(char** args){
    struct dirent *de;
    DIR *dr;

    if(args[0] == NULL) {
        dr = opendir(".");
 
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

        printf("%s\n", de->d_name); 
    }
  
    closedir(dr);     
    return; 
}

void escape() {
    printf("%s", "Goodbye =)");
    exit(0);
}

char* run_commands(char* cmd, char** args) {

    if(strcmp(cmd, "escape") == 0) {
        escape();
    } else if(strcmp(cmd, "ls") == 0) {
        list(args);
    }
}

