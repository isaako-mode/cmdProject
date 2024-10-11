#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./commands.h"
#include <dirent.h>
#include <unistd.h>
#include<fcntl.h> 

int main(int argc, char **argv) {
    
    //in the case that the user wants to move the file into a folder within their working directory, build the path to it.
    int dir_path_len = strlen(argv[2]) + strlen(argv[1]) + 2;
    char *into_dir = malloc(dir_path_len);
    if (into_dir == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }
    
    strcpy(into_dir, argv[2]);
    strcat(into_dir, "/");
    strcat(into_dir, argv[1]);


    // rename to the path (move the file to path)
    if (rename(argv[1], into_dir) == 0)
    {
        printf("%s%s", "File renamed moved into directory: ", argv[2]);
    }

    //rename file
    else if (rename(argv[1], argv[2]) == 0) {
        printf("rename successful");
    }
    else
    {
        printf("Unable to rename files. Please check files exist and you have permissions to modify files.\n");
    }

    free(into_dir);


    return 0;
}