#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

int main(int argc, char **argv) {

//ECHO
    if(argv[1] != NULL) {
        for(int i=1; argv[i] != NULL; i++) {
            printf("%s%c", argv[i], ' ');
        }
    }
    else {
        printf("Nothing to print!");
        return 1;
    }

    return 0;

}