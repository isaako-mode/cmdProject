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
#include "./queue.h"
#include <regex.h>


#define min(a, b) ((a) < (b) ? (a) : (b))
#define NUM_THREADS 4
#define MAX_LINES 500
#define MAX_LEN 100


typedef struct search {
    char* pattern;
    char** work_lines;
    char** output_lines;
    int index;
    int work;
    Queue* output_queue;
} search;

void *search_lines(void *arg) {
    search *search_obj = (search*)arg;

    regex_t reegex;
    int reg_match;
    reg_match = regcomp( &reegex, search_obj->pattern, 0);
    // Function call to create regex

    for(int i = search_obj->index; i < search_obj->index + search_obj->work; i++) {
        //printf("%d%s%d%s%s", search_obj->work, " THREAD ", search_obj->index," SEARCHLINE ", search_obj->work_lines[i-1]);

        if (search_obj->work_lines[i-1] != NULL && regexec( &reegex, search_obj->work_lines[i-1], 0, NULL, 0) == 0) {
            printf("MATCH\n");
            printf("%d%s%d%s%s", search_obj->work, " THREAD ", search_obj->index," SEARCHLINE ", search_obj->work_lines[i-1]);

            search_obj->output_lines[i-1] = malloc(sizeof(char)*MAX_LEN);
            enqueue(search_obj->output_queue, search_obj->output_lines[i-1]);
        }
    }
    regfree(&reegex);
    pthread_exit(search_obj->output_queue);
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
    char **lines;
    lines = malloc(sizeof(char*) * MAX_LINES);
    printf("%s", argv[2]);
    printf("%s", argv[1]);

    //track number of lines in the file/input
    int num_lines = 0;

    //open file
    FILE *fptr;
    fptr = fopen(argv[2], "r");

    //case if arg is a file
    if(fptr != NULL){


        int i = 0;

        while(!feof(fptr) && !ferror(fptr)) {
         
         lines[i] = malloc(sizeof(char)*MAX_LEN);
         if(!fgets(lines[i], MAX_LEN, fptr) == NULL) {
            i++;
            num_lines +=1;
         }

        }

        fclose(fptr);

    }

    //else its text from stdin!
    else {
        char *token = strtok(argv[2], "\n");

        int j = 0;
        while(token != NULL) {
            
            lines[j] = malloc(strlen(token) + 1);
            if (lines[j] == NULL) {
                perror("failed to allocate memory");
                exit(1);
            }

            strcpy(lines[j], token);

            token = strtok(NULL, "\n");
            j += 1;
            num_lines += 1;

        }
    }

    //work done by each thread
    int work = num_lines / NUM_THREADS;
    int remainder = num_lines % NUM_THREADS;

    //matched lines to output
    char** output_lines = malloc(sizeof(char*) * MAX_LINES);

    for (int i = 0; i < MAX_LINES; i++) {
        output_lines[i] = malloc(sizeof(char)*MAX_LEN);  // Initialize output lines to NULL
    }

    //create threads
    pthread_t* threads = malloc(sizeof(pthread_t)*NUM_THREADS);

    //struct for parameters to pass to threads
    search **search_objs = malloc(sizeof(search*)*(NUM_THREADS + 1));

    for (int i = 0; i < NUM_THREADS; i++) {
        search_objs[i] = malloc(sizeof(search));  // Allocate memory for each `search` object
    }
    search_objs[NUM_THREADS] = NULL;
    
    //FOR DEBUGGING
    for(int i = 0; i < num_lines; i++) {
    printf("%s", lines[i]);
    }

    //run the threads
    for(int i = 0; i < NUM_THREADS; i++) {
        //initialize search struct members
        search_objs[i]->pattern = strdup(argv[1]);
        search_objs[i]->work_lines = lines;
        search_objs[i]->output_lines = output_lines;
        search_objs[i]->index = ((i) * work + min(i, remainder)) + 1;  // Adjust the starting index
        search_objs[i]->work = work + (i < remainder ? 1 : 0);
        search_objs[i]->output_queue = create_queue();

        //create thread and run search_lines function
        pthread_create(&threads[i], NULL, search_lines, (void*)search_objs[i]);
    }

    Queue* output;

    //join the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], (void**)&output) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    
        if (output == NULL) {
        printf("Thread %d returned a NULL queue\n", i);
        continue;  // Skip if the thread returned NULL
        }

        while (output != NULL && output->front != NULL) {
        char* result = dequeue(output);
        if (result != NULL) {
            printf("%s\n", result);
            free(result); 
            }
        }

        free(output);
        output = NULL;
    }

    

    // for (int i = 0; i < MAX_LINES; i++) {
    //     if (output_lines[i] != NULL) {
    //         printf("%s", output_lines[i]);
    //         free(output_lines[i]);  // Free each output line
    //     }
    // }

    //free everything
    free(output_lines);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        free_queue(search_objs[i]->output_queue);
        free(search_objs[i]);
    }
     free(search_objs);
     free_queue(output);

    return 0;
}