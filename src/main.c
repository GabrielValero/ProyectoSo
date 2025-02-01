#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <ctype.h>

#include "Queue.h"
#include "Utils.h"
#include "Semaforo.h"

struct Queue scanList;
struct Queue scannedList;

int modeLibrary;  // 1 para biblioteca, 0 para ejecutable
int countElements = 0;

int main(int argc, char *argv[]) {
    
    int fileCount = 0;
    
    char execOp;
    struct stat info;
    int opt;
    char *initDirectory = NULL;
    
    

    initializeQueue(&scanList);
    initializeQueue(&scannedList);

    for(int i=0; i < 3; i++){
        opt = getopt(argc, argv, "t:d:m:");
        switch (opt)
        {
        case 't':
            if(isNumber(optarg)){
                if(atoi(optarg) <= 0){
                    printf("Ingrese una cantidad de semaforos positivo\n");
                    exit(EXIT_FAILURE);
                }
                sem_init(&semMax, 0, atoi(optarg)); //Modificalo como consideres
                sem_init(&semCount, 0, 1);
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
            if(optarg[0] == 'e'){
                modeLibrary = 0;
            }else if(optarg[0] == 'l'){
                modeLibrary = 1;
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
    if (initDirectory) {
        scanDirectory(initDirectory, &scanList, &countElements);
    }
    struct file files[countElements];
    pthread_t thread[countElements];
    PthreadData threadInfo[countElements];
    int i = 0;
    while (!isEmpty(&scanList)) {
        char *filePath = dequeue(&scanList);
        threadInfo[i].fileCount = &fileCount;
        threadInfo[i].filePath = filePath;
        threadInfo[i].modeLibrary = modeLibrary;
        threadInfo[i].files = malloc(sizeof(files));
        threadInfo[i].files = files;
        pthread_create(&thread[i], NULL, &dVerify, (void*) &threadInfo[i]);
        i+=1;
    }
    while(i!= 0){
        //free(threadInfo[i].files);
        pthread_join(thread[i], NULL);
        i-=1;
    }
    findDuplicates(files, fileCount, &scannedList);

    sem_destroy(&semMax);
    sem_destroy(&semCount);
    return 0;
}

