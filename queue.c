#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./queue.h"
#include <unistd.h>
#include<fcntl.h> 
#include <stdio.h>


Queue* create_queue() {
    Queue* myQueue = (Queue*)malloc(sizeof(Queue));
    if(myQueue == NULL) {
        printf("Failed to allocate queue");
        exit(1);
    }

    myQueue->front = NULL;
    myQueue->back = NULL;

    return myQueue;
}

/*
Enqueue:
take new node and make its next pointer the back of the queue

Dequeue:
make the node behind the front the new front, then free the old front
*/

void enqueue(Queue* queue, char *data) {
    
    if(data == NULL) {
        printf("Cannot enqueue NULL string");
        return;
    }

    if(queue == NULL) {
        printf("NULL Queue Passed");
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    if(newNode == NULL) {
        perror("Could not allocate memory for new Node");
        exit(1);
        }

    newNode->data = (char*)calloc(strlen(data) + 1,1);
    if (newNode->data == NULL) {
        perror("Could not allocate memory for node data");
        free(newNode); 
        exit(1);
    }

    strcpy(newNode->data, data);
    
    //if queue is empty
    if(queue->back == NULL) {
        queue->front = newNode;
        queue->back = newNode;
    }

    //standard enqueue
    else {
        queue->back->next = newNode;
        queue->back = newNode;
    }
}

char* dequeue(Queue* queue) {
    
    if(queue == NULL || queue->front == NULL) {
        printf("Empty queue!");
        return NULL;
    }
    
    if(queue->front->data == NULL) {
        printf("Front has NULL string!");
        return NULL;
    }
    
    char* returnString = queue->front->data;
    Node* tempNode = queue->front;
    queue->front = queue->front->next;

    if(queue->front == NULL) {
        queue->back = NULL;
    }

    free(tempNode);
    return returnString;
    
    
}

void free_queue(Queue* queue) {
    // if (queue == NULL) {
    //     printf("Queue is NULL");
    //     return;
    // }
    while(queue->front != NULL) {
        char* temp = dequeue(queue);
        free(temp);
    }

    free(queue);
}

