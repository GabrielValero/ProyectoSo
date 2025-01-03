#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cola.h"


struct nodo {
    char *directorio; 
    struct nodo *sig;
};

struct Cola {
    struct nodo *raiz;
    struct nodo *fondo;
};

// Inicializa la cola
void inicializarCola(struct Cola *cola)
{
    cola->raiz = NULL;
    cola->fondo = NULL;
}

// Devuelve si la cola está vacía
int vacia(struct Cola *cola)
{
    return cola->raiz == NULL;
}

//Inserta elementos a la cola

void insertar(char *x, struct Cola *cola)
{
    struct nodo *nuevo = malloc(sizeof(struct nodo));
    nuevo->directorio = malloc(strlen(x) + 1); 
    strcpy(nuevo->directorio, x);
    nuevo->sig = NULL;

    if (vacia(cola)) {
        cola->raiz = nuevo;
        cola->fondo = nuevo;
    } else {
        cola->fondo->sig = nuevo;
        cola->fondo = nuevo;
    }
}

//Extrae elementos de la cola
char* extraer(struct Cola *cola)
{
    if (!vacia(cola)) {
        char *informacion = cola->raiz->directorio;
        struct nodo *aux = cola->raiz;

        if (cola->raiz == cola->fondo) {
            cola->raiz = NULL;
            cola->fondo = NULL;
        } else {
            cola->raiz = cola->raiz->sig;
        }
        free(aux);
        return informacion;
    } else {
        return NULL;
    }
}


//Libera la memoria
void liberar(struct Cola *cola)
{
    struct nodo *recorrido = cola->raiz;
    struct nodo *aux;

    while (recorrido != NULL) {
        aux = recorrido;
        recorrido = recorrido->sig;
        free(aux->directorio);
        free(aux);
    }
    cola->raiz = NULL;
    cola->fondo = NULL;
}