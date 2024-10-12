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
    int dir_path_len = 0;
    for(int i = 1; argv[i] != NULL; i++) {
        dir_path_len += 1;
    }
    
    dir_path_len += 2;

    char *dir_path = malloc(sizeof(char*)*dir_path_len);

    if (dir_path == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }
    
    //calculate relative path
    strcpy(dir_path, argv[dir_path_len-2]);
    strcat(dir_path, "/");
    strcat(dir_path, argv[1]);

    // rename to the path (move the file to path)
    if (rename(argv[1], dir_path) == 0)
    {
        // for handling moving multiple files
        int path_size;
        for(int i = 2; i < dir_path_len-2; i++) {

            //calculate relative path (add/sub 2 to account for directory name at end and null terminator)
            path_size = strlen(argv[dir_path_len-2]) + strlen(argv[i]) + 2;
            memset(dir_path, '\0', path_size*sizeof(char*));
            strcpy(dir_path, argv[dir_path_len-2]);
            strcat(dir_path, "/");
            strcat(dir_path, argv[i]);

            if(rename(argv[i], dir_path) == 0) {

                continue;
            }

            else {
                printf("%s%s", "No such file named ", argv[i]);
                free(dir_path);
                exit(1);
            }
        }

        printf("%s%s", "File renamed moved into directory: ", argv[2]);
    }

    //case of two files (rename)
    else if (rename(argv[1], argv[2]) == 0) {
        printf("rename successful");
    }
    else
    {
        printf("Unable to rename files. Please check files exist and you have permissions to modify files.\n");
    }

    free(dir_path);


    return 0;
}