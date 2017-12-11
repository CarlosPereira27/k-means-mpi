#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include "database_handler.h"
#include "array.h"

#define BIN_EXTENSION ".bin"
#define CLASS_EXTENSION "_class.csv"
#define BIN_EXTENSION_LENGTH 5
#define CLASS_EXTENSION_LENGTH 11

char* getNewDatabaseName(char* name);
char* getNewDatabaseClassName(char* name_database);
int getClassCod(Array array_class, const char* class_name);
void writeDatabaseClassCode(char* name_database, Array array_class);
void writeDatabaseBin(char* name_database, Array array_class, Database database);

char* getNewDatabaseName(char* name) {
    int lenName = strlen(name);
    char* newName = (char*) malloc((lenName + BIN_EXTENSION_LENGTH) * sizeof(char));
    strcpy(newName, name);
    strcpy(&(newName[lenName]), BIN_EXTENSION);
    return newName;
}

char* getNewDatabaseClassName(char* nameDatabase) {
    int lenName = strlen(nameDatabase);
    char* newNameClass = (char*) malloc((lenName + CLASS_EXTENSION_LENGTH) * sizeof(char));
    strcpy(newNameClass, nameDatabase);
    strcpy(&(newNameClass[lenName]), CLASS_EXTENSION);
    return newNameClass;
}

int getClassCod(Array array_class, const char* class_name) {
    int i = 0;
    int N = array_class->used;
    for (i = 0; i < N; i++) {
        const char* class_name_i = (char*) array_class->data[i];
        if (strcmp(class_name_i, class_name) == 0) {
            return i;
        }
    }
    addArray(array_class, (void*) class_name);
    return N;
}

void writeDatabaseClassCode(char* name_database, Array array_class) {
    char* new_database_class_name = getNewDatabaseClassName(name_database);
    int i;
    int N = array_class->used;
    FILE* file = fopen(new_database_class_name, "w+");
    if (file == NULL) {
        printf("Erro! Não foi possível criar o arquivo '%s'.\n", new_database_class_name);
        exit(0);
    }
    fprintf(file, "class_name,class_code\n");
    for (i = 0; i < N; i++) {
        const char* className = (char*) array_class->data[i];
        fprintf(file, "%s,%d\n", className, i);
    }
    fclose(file);
    printf("Os dados das classes da base de dados '%s' ", name_database);
    printf("foram salvos com sucesso no arquivo '%s'.\n", new_database_class_name);
}

void writeDatabaseBin(char* name_database, Array array_class, Database database) {
    char* new_database_name = getNewDatabaseName(name_database);
    int N = database->instances->used;
    int features_length = database->features_length;
    int i;
    FILE* file = fopen(new_database_name, "w");
    void** instances = database->instances->data;
    if (file == NULL) {
        printf("Erro! Não foi possível criar o arquivo '%s'.\n", new_database_name);
        exit(0);
    }
    printf("features_length -> %d\n", features_length);
     printf("N -> %d\n", N);
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        int classCode = getClassCod(array_class, instance->clazz);
        fwrite(&classCode, sizeof(int), 1, file);
        fwrite(instance->features, sizeof(double), features_length, file);
    }
    fclose(file);
    printf("Os dados binários da base de dados '%s' ", name_database);
    printf("foram salvos com sucesso no arquivo '%s'.\n", new_database_name);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("A quantidade de argumentos é inválida!\n");
        exit(0);
    }
    Database database = readDatabase(argv[1], atoi(argv[2]));
    Array array_class = makeArray(20);
    writeDatabaseBin(argv[1], array_class, database);
    writeDatabaseClassCode(argv[1], array_class);
    return 0;
} 