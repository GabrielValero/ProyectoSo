#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"

// Inicializa la cola
void initializeQueue(struct Queue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

// Devuelve si la cola está vacía
int isEmpty(struct Queue *queue) {
    return queue->front == NULL;
}

// Inserta elementos a la cola
void enqueue(char *x, struct Queue *queue) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->directory = malloc(strlen(x) + 1);
    strcpy(newNode->directory, x);
    newNode->next = NULL;

    if (isEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Extrae elementos de la cola
char* dequeue(struct Queue *queue) {
    if (!isEmpty(queue)) {
        char *info = queue->front->directory;
        struct node *temp = queue->front;

        if (queue->front == queue->rear) {
            queue->front = NULL;
            queue->rear = NULL;
        } else {
            queue->front = queue->front->next;
        }
        free(temp);
        return info;
    } else {
        return NULL;
    }
}

// Libera la memoria
void freeQueue(struct Queue *queue) {
    struct node *current = queue->front;
    struct node *temp;

    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp->directory);
        free(temp);
    }
    queue->front = NULL;
    queue->rear = NULL;
}
