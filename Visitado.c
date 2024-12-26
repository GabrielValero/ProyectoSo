#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>

struct nodo {
    char *directorio; 
    struct nodo *sig;
};

struct nodo *raiz = NULL;
struct nodo *fondo = NULL;

int vacia()
{
    if (raiz == NULL)
        return 1;
    else
        return 0;
}

void insertarVisitado(char *x)
{
    struct nodo *nuevo;
    nuevo = malloc(sizeof(struct nodo));
    nuevo->directorio = malloc(strlen(x) + 1); 
    strcpy(nuevo->directorio, x);
    nuevo->sig = NULL;
    if (vacia())
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

char* extraerVisitado()
{
    if (!vacia())
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

void liberar()
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
