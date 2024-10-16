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

    newNode->data = (char*)malloc(strlen(data) + 1);
    if (newNode->data == NULL) {
        perror("Could not allocate memory for node data");
        free(newNode); 
        exit(1);
    }

    strcpy(data, newNode->data);
    
    //if queue is empty
    if(queue->back == NULL) {
        queue->front = newNode;
        queue->back = newNode;
    }

    //standard enqueue
    else {
        newNode->next = queue->back;
        queue->back = newNode->next;
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

    free(tempNode);
    return returnString;
    
    
}

void free_queue(Queue* queue) {
    while(queue->front != NULL) {
        char* temp = dequeue(queue);
        free(temp);
    }

    free(queue);
}

