#ifndef QUEUE_H
#define QUEUE_H
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h> 
#include <stdio.h>

 typedef struct node {
    char* data;
    struct node* next;
} Node;

typedef struct queue {
    Node* front;
    Node* back;
} Queue;


/*
Enqueue:
take new node and make its next pointer the back of the queue

Dequeue:
make the node behind the front the new front, then free the old front
*/

Queue* create_queue();
void enqueue(Queue* queue, char *data);
char* dequeue(Queue* queue);
void free_queue(Queue* queue);

#endif