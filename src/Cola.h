// Visitado.h
#ifndef COLA_H
#define COLA_H
struct nodo {
    char *directorio; 
    struct nodo *sig;
};
int vacia(struct nodo *);
void insertar(char *, struct nodo *, struct nodo *);
char* extraer(struct nodo *, struct nodo *);
void liberar(struct nodo *);

#endif
