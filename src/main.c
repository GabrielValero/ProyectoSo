#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>
#include <ctype.h>
#include <sys/types.h>

#include "Queue.h"

struct Queue scanList;
struct Queue scandedList;

sem_t semMax;

void scanDirectory(char *);
int isNumber(const char *);

int main(int argc, char *argv[]){
    int pipefd[2];
    int doubleCount = 0;
    int opt;
    char execOp;
    pid_t pid;
    struct stat info;
    char *initDirectory;
    
    initializeQueue(&scanList);
    initializeQueue(&scandedList);

    for(int i=0; i < 3; i++){
        opt = getopt(argc, argv, "t:d:m:");
        switch (opt)
        {
        case 't':
            if(isNumber(optarg)){
                sem_init(&semMax, 0, atoi(optarg)); //Modificalo como consideres
            }else{
                printf("Ingrese una cantidad de semaforos positivo\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            initDirectory = optarg;

            if(stat(initDirectory, &info) == 0){
                //si es directorio que haga la recursividad
                if(!S_ISDIR(info.st_mode)){
                    printf("La direccion no es un directorio\n");
                    exit(EXIT_FAILURE);
                }
            }else{
                printf("No existe la direccion\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'm':
            if(optarg[0] == 'e' || optarg[0] == 'l'){
                execOp = optarg[0];
            }else{
                printf("Error al escoger entre modo ejecutable y modo biblioteca\n");
                exit(EXIT_FAILURE);
            }
            break;
        
        default:
            printf("Error en los flags\n");
            exit(1);
            break;
        }
    }

    scanDirectory(initDirectory);
 
    if(pipe(pipefd) == -1){
        perror("Error al crear la tuberia");
        exit(EXIT_FAILURE);
    }

    pid = vfork();
    if(pid < 0){
        perror("Error al crear otro proceso");
        exit(EXIT_FAILURE);
    }else if (pid == 0){
        close(pipefd[0]);
        while(!isEmpty(&scanList)){
            dequeue(&scanList);
            doubleCount++;
        }
        write(pipefd[0], &doubleCount, sizeof(doubleCount));
        close(pipefd[1]);
        _exit(0);
    }else{
        close(pipefd[1]);
        read(pipefd[0], &doubleCount, sizeof(doubleCount));

        printf("Hay %d elementos \n", doubleCount);
        close(pipefd[0]);
        wait(NULL);
    }

    sem_destroy(&semMax);
    return 0;
}

int isNumber(const char *str){

    while (*str){
        if(!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

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
                //printf("Directorio: %s \n", completeDir);
                scanDirectory(completeDir);
            }else if(S_ISREG(info.st_mode)){
                //Sino ps que solo imprima la direccion, aqui va el uso de la cola
                //printf("Archivo: %s \n", completeDir);
                enqueue(completeDir, &scanList);
            }
        }
    }
    closedir(dir);
};