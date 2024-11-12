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
#define MAX_CHARS 1000

//struct for attributes each thread needs
typedef struct search {
    char* pattern;
    char** work_lines;
    char** output_lines;
    int index;
    int work;
    Queue* output_queue;
} search;

//thread function that runs regex pattern on the work given to the threads
void *search_lines(void *arg) {
    search *search_obj = (search*)arg;

    regex_t reegex;
    int reg_match;
    reg_match = regcomp(&reegex, search_obj->pattern, 0);
    // Function call to create regex

    for(int i = search_obj->index; i < search_obj->index + search_obj->work; i++) {
        //printf("%d%s%d%s%s", search_obj->work, " THREAD ", search_obj->index," SEARCHLINE ", search_obj->work_lines[i-1]);

        if (search_obj->work_lines[i-1] != NULL && regexec( &reegex, search_obj->work_lines[i-1], 0, NULL, 0) == 0) {
            //printf("MATCH\n");
            //printf("%d%s%d%s%s", search_obj->work, " THREAD ", search_obj->index," SEARCHLINE ", search_obj->work_lines[i-1]);

            //search_obj->work_lines[i-1] = malloc(sizeof(char)*MAX_LEN);
            enqueue(search_obj->output_queue, search_obj->work_lines[i-1]);
            //printf("%s%d%s%s", "THREAD: ", search_obj->index, " ENQUEUED: " , search_obj->output_queue->front->data);
        }
    }
    regfree(&reegex);
    pthread_exit((void*)search_obj->output_queue);
}


int main(int argc, char **argv) {

    char** lines;
    lines = malloc(sizeof(char*) * MAX_LINES);

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

        //Need to read from STDIN instead here
        char input_buffer[MAX_CHARS];

        fgets(input_buffer, sizeof(input_buffer), stdin);

        if(input_buffer == NULL) {
            printf("Bad input to STDIN (GREP)");
            exit(1);
        }

        //set ending newline to null terminator
        input_buffer[strcspn(input_buffer, "\n")] = '\0';

        char *token = strtok(input_buffer, "\n");

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
    char** output_lines = malloc(sizeof(char*) * (MAX_LINES));

    for (int i = 0; i < MAX_LINES; i++) {
        output_lines[i] = malloc(sizeof(char) * (MAX_LEN));  // Initialize output lines to NULL
    }

    //create threads
    pthread_t* threads = malloc(sizeof(pthread_t)*NUM_THREADS);

    //struct for parameters to pass to threads
    search **search_objs = malloc(sizeof(search*)*(NUM_THREADS));

    for (int i = 0; i < NUM_THREADS; i++) {
        search_objs[i] = malloc(sizeof(search));  // Allocate memory for each `search` object
    }
    //search_objs[NUM_THREADS] = NULL;
    
    //FOR DEBUGGING
    // for(int i = 0; i < num_lines; i++) {
    // printf("%s", lines[i]);
    // }

    //run the threads

    char* pattern;

    if (argv[1] == NULL) {
        printf("Invalid or missing pattern");
        exit(1);
    }
    pattern = argv[1];

    for(int i = 0; i < NUM_THREADS; i++) {
        //initialize search struct members
        search_objs[i]->pattern = pattern;
        search_objs[i]->work_lines = lines;
        search_objs[i]->output_lines = output_lines;
        search_objs[i]->index = ((i) * work + min(i, remainder)) + 1;  // Adjust the starting index
        search_objs[i]->work = work + (i < remainder ? 1 : 0);
        search_objs[i]->output_queue = create_queue();

        //create thread and run search_lines function
        pthread_create(&threads[i], NULL, search_lines, (void*)search_objs[i]);
    }

    //void* output = NULL;
    Queue* match_queue;

    //join the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], (void**)(&match_queue)) != 0) {
            perror("Failed to join thread");
            return 1;
        }
        //match_queue = (Queue*)output;
        if (match_queue == NULL) {   
            printf("Thread %d returned a NULL queue\n", i);
            continue;  // Skip if the thread returned NULL
        }

        while (match_queue->front != NULL && match_queue->front->data != NULL) {

            char* result = dequeue(match_queue);
            
            if (result != NULL) {
                printf("%s", result);
                free(result);
                result = NULL;
                }
            }
            free_queue(match_queue);
            match_queue = NULL;
        }

    

    //free everything
    for(int i = 0; i < MAX_LINES; i++) {
        if(output_lines[i] != NULL) {
            free(output_lines[i]);
        }
    }
    free(output_lines);

    for(int i = 0; i < num_lines; i++) {
        if(lines[i] != NULL) {
            free(lines[i]);
        }
    }
    free(lines);

    for (int i = 0; i < NUM_THREADS; i++) {
    //     free_queue(search_objs[i]->output_queue);
        free(search_objs[i]);  
    }
    free(search_objs);
    free(threads);

    return 0;
}