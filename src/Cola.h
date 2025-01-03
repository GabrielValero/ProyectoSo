//Cola.h
#ifndef COLA_H
#define COLA_H

// Nodo que representa un elemento en la cola
struct node {
    char *directory;
    struct node *next;
};

// Funciones de la cola
int isEmpty(struct node *);
void enqueue(char *, struct node *, struct node *);
char* dequeue(struct node *, struct node *);
void freeQueue(struct node *);

#endif