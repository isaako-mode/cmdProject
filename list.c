#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

int main(int argc, char **argv) {

        //Define directory struct
    struct dirent *de;
    DIR *dr;

    if(argv[1] == NULL) {
        dr = opendir(".");
    } 
    else if(strcmp(argv[1], "~") == 0) {
        dr = opendir("/home/isaakpi");
    }

    else {
        dr = opendir(argv[1]);
    }

    if (dr == NULL) {
        printf("Could not open current directory" ); 
        return 1;
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
    return 0; 
}