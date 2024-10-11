#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    struct stat st = {0};

    if (stat(argv[1], &st) == -1) {
        mkdir(argv[1], 0700);
    }

}