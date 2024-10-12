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
#include <pthread.h>

#define NUM_THREADS 4
#define MAX_LINES 500
#define MAX_LEN 100


typedef struct search {
    char* pattern;
    char** search_lines;
    char** output_lines;
    int index;
    int work;
} search;

void *search_lines(void *arg) {
    pthread_mutex_t lock;
    search *search_obj = (search*)arg;

    for(int i = search_obj->index; i < i + search_obj->work; i++) {
        if (strstr(search_obj->pattern, search_obj->search_lines[i]) != NULL) {

            search_obj->output_lines[i] = malloc(sizeof(char*)*MAX_LEN);
            strcpy(search_obj->output_lines[i], search_obj->search_lines[i]);
        }
    }

    pthread_exit(NULL);  // Return the result
}


int main(int argc, char **argv) {
    //PLAN:
    //read the file
    //get number of lines
    //calc number of threads (how will this scale w file size?)

    //each thread will get (number of lines) / (number of threads) lines
    //each thread will check its lines for the regex pattern

    //first threads will write their work in earlier indexes of the output bc that will keep the order of lines w regex pattern

    //data to divy up the work for threads
    char lines[MAX_LINES][MAX_LEN];
    //memset(lines, '\0', sizeof(lines));

    int num_lines = 0;

    //open file
    FILE *fptr;
    fptr = fopen(argv[2], "r");

    //case if arg is a file
    if(fptr != NULL){

        int i = 0;

        while(!feof(fptr) && !ferror(fptr)) {
         
         if(!fgets(lines[i], MAX_LEN, fptr) == NULL) {
            i++;
            num_lines +=1;
         }

        }

        fclose(fptr);

    }

    //else its text!
    else {
        char *token = strtok(argv[2], "\n");

        int j = 0;
        while(token != NULL) {
            strcpy(lines[j], token);

            if (lines[j] == NULL) {
                perror("failed to allocate memory");
                exit(1);
            }

            token = strtok(NULL, "\n");
            j += 1;
            num_lines += 1;

        }
    }

    // //work done by each thread
    int work = num_lines / NUM_THREADS;
    char** output_lines = malloc(sizeof(char**)*MAX_LINES);

    pthread_t* threads = malloc(sizeof(pthread_t)*NUM_THREADS);
    search **search_objs = malloc(sizeof(search)*NUM_THREADS);
//     for(int i = 0; i < num_lines; i++) {
//     printf("%s", lines[i]);
// }
    for(int i = 0; i < NUM_THREADS; i++) {
        
        search_objs[i]->pattern = argv[1];
        search_objs[i]->search_lines = lines;
        search_objs[i]->output_lines = output_lines;
        search_objs[i]->index = i;
        search_objs[i]->work = work;

        printf("%s", search_objs[i]->pattern);
        // printf("%s", search_objs[i]->search_lines);
        // printf("%s", search_objs[i]->output_lines);
        printf("%d", search_objs[i]->work);


        pthread_create(&threads[i], NULL, search_lines, (void*)&search_objs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    int num_results = 0;
    for(int i = 0; output_lines[i] != NULL; i++) {
        num_results += 1;
        printf("%s", output_lines[i]);
    }

    free(output_lines);
    free(search_objs);

    return 0;
}