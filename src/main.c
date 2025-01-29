#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <semaphore.h>
#include <sys/types.h>
#include "Queue.h"
#include "md5-lib/global.h"
#include "md5-lib/md5.h"

#define MAX_FILES 500  // Máximo número de archivos a almacenar

struct Queue scanList;
struct Queue scannedList;

sem_t semMax;
int modeLibrary = 0;  // 1 para biblioteca, 0 para ejecutable

struct file {
    char directory[1024]; // Ruta del archivo
    char hash[33];        // Hash MD5
    int check;            // Indicador de verificación
};


// Prototipos
void scanDirectory(char *);
void dVerify(char *filePath, struct file files[], int *fileCount);
int addFile(struct file files[], int count, const char *directory, const char *hash);
void findDuplicates(struct file files[], int count);

int main(int argc, char *argv[]) {
    int fileCount = 0;
    struct file files[MAX_FILES];

    int opt;
    char *initDirectory = NULL;

    initializeQueue(&scanList);
    initializeQueue(&scannedList);

    while ((opt = getopt(argc, argv, "t:d:m:")) != -1) {
        switch (opt) {
            case 't':
                printf("-t %s \n", optarg);
                sem_init(&semMax, 0, atoi(optarg)); // Modifícalo según sea necesario
                break;
            case 'd':
                printf("-d %s \n", optarg);
                initDirectory = optarg;
                break;
            case 'm':
                printf("-m %s \n", optarg);
                if (strcmp(optarg, "l") == 0) {
                    modeLibrary = 1;  // Activar modo biblioteca
                } else if (strcmp(optarg, "e") == 0) {
                    // Modo ejecutable activado
                }
                break;
            default:
                break;
        }
    }

    if (initDirectory) {
        scanDirectory(initDirectory);
    }

    while (!isEmpty(&scanList)) {
        char *filePath = dequeue(&scanList);
        dVerify(filePath, files, &fileCount);
    }

    if (modeLibrary) {
        findDuplicates(files, fileCount);
    }

    sem_destroy(&semMax);
    return 0;
}

void dVerify(char *filePath, struct file files[], int *fileCount) {
    if (modeLibrary) {
        // Modo biblioteca
        char hashValue[33];
        if (MDFile(filePath, hashValue)) {
            //printf("Archivo: %s - Hash MD5: %s\n", filePath, hashValue);
            *fileCount = addFile(files, *fileCount, filePath, hashValue);
        } else {
            printf("Error al calcular hash MD5 de %s\n", filePath);
        }
    } else {
        // Modo ejecutable
        int pipeChild[2], pipeParent[2];
        if(pipe(pipeChild) == -1){
            perror("Error al crear la tuberia");
            exit(EXIT_FAILURE);
        }
        pid_t pid;
        pid = vfork();
        if(pid < 0){
            perror("Error al crear otro proceso");
            exit(EXIT_FAILURE);
        }else if (pid == 0){
            char *str = dequeue(&scanList);
            close(pipeChild[0]);
            dup2(pipeChild[1], STDOUT_FILENO);
            execl("./md5-app/md5", "md5", str, NULL);
            close(pipeChild[1]);
            exit(EXIT_FAILURE);
        }else{
            char buffer[33];
            close(pipeChild[1]);
            read(pipeChild[0], &buffer, sizeof(buffer));
            printf("Finalizo con: %s\n", buffer);
            close(pipeChild[0]);
            wait(NULL);
        }
    }
}

void scanDirectory(char *route) {
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
void findDuplicates(struct file files[], int count) {
    for (int i = 0; i < count; i++) {
        if (files[i].check == 0) {
            continue; // Si ya fue revisado, lo saltamos
        }

        int duplicates = 0; // Contador de duplicados

        for (int j = i + 1; j < count; j++) {
            if (files[j].check == 1 && strcmp(files[i].hash, files[j].hash) == 0) {
                printf("%s es duplicado de %s\n", files[j].directory, files[i].directory);
                files[j].check = 0; // Marcamos como revisado
                duplicates++;
            }
        }

        if (duplicates > 0) {
            printf("Número de duplicados de: %d\n", duplicates);
            files[i].check = 0; // Tambien marcamos el del principio como revisado
        }
    }
}
