#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

int main(int argc, char **argv) {
    FILE *fptr;

    if(argv[1] != NULL) {

        fptr = fopen(argv[1], "r");
    }

    char text[500];

    if(fptr != NULL){

        while(fgets(text, 500, fptr)) {
            printf("%s", text);
        }
    }
    else {
        printf("no file foo");
        return 1;
    }

    return 0;
}