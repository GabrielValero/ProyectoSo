#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include "Queue.h"
#include "Utils.h"
#include "Semaforo.h"

void *dVerify(void *arg) {
    PthreadData *data = (PthreadData *) arg;
    sem_wait(&semMax);
    char hashValue[40];
    
    if (data->modeLibrary) {
    // Modo biblioteca
        if (MDFile(data->filePath, hashValue)) {
            sem_wait(&semCount);
            *(data->fileCount) = addFile(data->files, *(data->fileCount), data->filePath, hashValue);
            sem_post(&semCount);
        } else {
            printf("Error al calcular hash MD5 de %s\n", data->filePath);
        }
    } else {
        //Modo ejecutable
        int pipeChild[2], pipeParent[2];
        if(pipe(pipeChild) == -1){
            perror("Error al crear la tuberia");
            exit(EXIT_FAILURE);
        }
        pid_t pid;
        pid = fork();
        if(pid < 0){
            perror("Error al crear otro proceso");
            exit(EXIT_FAILURE);
        }else if (pid == 0){
            close(pipeChild[0]);
            dup2(pipeChild[1], STDOUT_FILENO);
            close(pipeChild[1]);
            execl("./md5-app/md5", "md5", data->filePath, NULL);
            exit(EXIT_FAILURE);
        }else{
            close(pipeChild[1]);
            wait(NULL);
            ssize_t bytesRead = read(pipeChild[0], &hashValue, sizeof(hashValue)-1);
            
            //Para eliminar cualquier bit basura despues del hash
            if(bytesRead > 0){
                hashValue[bytesRead] = '\0';
            }else{
                hashValue[0] = '\0'; // Si no lee nada eliminar datos basura
            }
            sem_wait(&semCount);
            *(data->fileCount) = addFile(data->files, *(data->fileCount), data->filePath, hashValue);
            sem_post(&semCount);
            close(pipeChild[0]);
        }
       

    }
    sem_post(&semMax);
}

void scanDirectory(char *route,struct Queue *scanList, int *countElement) {
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
                scanDirectory(completeDir, scanList, countElement);
            }else if(S_ISREG(info.st_mode)){
                //Sino ps que solo imprima la direccion, aqui va el uso de la cola
                *countElement+=1;
                enqueue(completeDir, scanList);
            }
        }
    }
    closedir(dir);
}

// Agregar archivos al arreglo
int addFile(struct file files[], int count, const char *directory, const char *hash) {
    if (count >= MAX_FILES) {
        fprintf(stderr, "Error: Se alcanzó el límite máximo de archivos\n");
        return count;
    }

    strncpy(files[count].directory, directory, sizeof(files[count].directory));
    strncpy(files[count].hash, hash, sizeof(files[count].hash));
    files[count].check = 1;  // Marcamos el archivo como no revisado
    return count + 1;        // Nuevo tamaño del arreglo
}

// Buscar y contar archivos duplicados
void findDuplicates(struct file files[], int count, struct Queue *scannedList) {
    int duplicates = 0; // Contador de duplicados
    int i,j;
    for (i = 0; i < count; i++) {
        if (files[i].check == 0) {
            continue; // Si ya fue revisado, lo saltamos
        }
        
        enqueue(files[i].directory, scannedList); // se agrega a la cola
        

        for (j = i + 1; j < count; j++) {
            if (files[j].check == 1 && strcmp(files[i].hash, files[j].hash) == 0) {
                char *name1 = strrchr(files[j].directory, '/');
                name1 = name1 ? name1 + 1 : files[j].directory;
                char *name2 = strrchr(files[i].directory, '/');
                name2 = name2 ? name2 + 1 : files[i].directory;
                printf("%s es duplicado de %s\n", name1, name2);
                files[j].check = 0; // Marcamos como revisado
                duplicates++;
            }
        }
            
        files[i].check = 0; // Marcamos el del principio como revisado
        
    }
    printf("Número de duplicados: %d\n", duplicates);
}

int isNumber(const char *str){

    while (*str){
        if(!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}