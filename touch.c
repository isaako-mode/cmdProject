#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

int main(int argc, char **argv) {
    int fd;

    fd = open(argv[1], O_WRONLY | O_CREAT, 0644);

    close(fd);

    return 0;

}