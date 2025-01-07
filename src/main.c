#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include "Queue.h"
#include <string.h>

struct Queue scanList;
struct Queue scandedList;

void scanDirectory(char *route){
    //abre la direccion
    DIR * dir = opendir(route);
    
    //valida que se haya abierto bien
    if(dir == NULL){
        perror("No se pudo abrir el directorio \n");
        return;
    }
    struct dirent *element;
    struct stat info;
    char completeDir[1024];
    //Revisa que lea todos los elementos
    while((element = readdir(dir)) != NULL){
        //saca los archivos . y ..
        if(strcmp(element->d_name, ".") == 0 || strcmp(element->d_name, "..") == 0){
            continue;
        }
        //Ajusta la direccion completa del elemento
        snprintf(completeDir, sizeof(completeDir), "%s/%s", route, element->d_name);
        //revisa si es un directorio o un archivo
        if(stat(completeDir, &info) == 0){
            //si es directorio que haga la recursividad
            if(S_ISDIR(info.st_mode)){
                printf("Directorio: %s \n", completeDir);
                scanDirectory(completeDir);
            }else if(S_ISREG(info.st_mode)){
                //Sino ps que solo imprima la direccion, aqui va el uso de la cola
                printf("Archivo: %s \n", completeDir);
                enqueue(completeDir, &scanList);
            }
        }
    }
    closedir(dir);
};

int main(int argc, char *argv[]){
    initializeQueue(&scanList);
    initializeQueue(&scandedList);

    scanDirectory(argv[1]);

    return 0;
}