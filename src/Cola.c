#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Cola.h"

int vacia(struct nodo *raiz)
{
    if (raiz == NULL)
        return 1;
    else
        return 0;
}

void insertar(char *x, struct nodo *raiz, struct nodo *fondo)
{
    struct nodo *nuevo;
    nuevo = malloc(sizeof(struct nodo));
    nuevo->directorio = malloc(strlen(x) + 1); 
    strcpy(nuevo->directorio, x);
    nuevo->sig = NULL;
    if (vacia(raiz))
    {
        raiz = nuevo;
        fondo = nuevo;
    }
    else
    {
        fondo->sig = nuevo;
        fondo = nuevo;
    }
}

char* extraer(struct nodo *raiz, struct nodo *fondo)
{
    if (!vacia(raiz))
    {
        char *informacion = raiz->directorio;
        struct nodo *aux = raiz;
        if (raiz == fondo)
        {
            raiz = NULL;
            fondo = NULL;
        }
        else
        {
            raiz = raiz->sig;
        }
        free(aux);
        return informacion;
    }
    else
        return NULL;
}

void liberar(struct nodo *raiz)
{
    struct nodo *recorrido = raiz;
    struct nodo *aux;
    while (recorrido != NULL)
    {
        aux = recorrido;
        recorrido = recorrido->sig;
        free(aux->directorio);
        free(aux); 
    }
}
