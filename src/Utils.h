#ifndef UTILS_H
#define UTILS_H

#define MAX_FILES 500  // Maximo numero de archivos a almacenar



struct file {
    char directory[1024]; // Ruta del archivo
    char hash[33];        // Hash MD5
    int check;            // Indicador de verificacion
};
typedef struct{
    char *filePath;
    struct file *files;
    int *fileCount;
    int modeLibrary;
}PthreadData;

int isNumber(const char *);
void scanDirectory(char *, struct Queue *, int*);
void *dVerify(void *);
int addFile(struct file files[], int, const char *, const char *);
void findDuplicates(struct file files[], int, struct Queue *);

#endif