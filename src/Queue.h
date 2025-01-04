#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
struct node {
    char *directory;
    struct node *next;
};

struct Queue {
    struct node *front;
    struct node *rear;
};

// Funciones de la cola
void initializeQueue(struct Queue *queue);
int isEmpty(struct Queue *queue);
void enqueue(char *x, struct Queue *queue);
char* dequeue(struct Queue *queue);
void freeQueue(struct Queue *queue);

#endif
