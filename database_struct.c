#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include "database_struct.h"

Database makeDatabase(const char* path_database, int length) {
    Database database = (Database) malloc(sizeof(S_Database));
    int lengthName = strlen(path_database) + 1;
    database->path_database = (char*) malloc(sizeof(char) * lengthName);
    strcpy(database->path_database, path_database);
    database->instances = makeArray(length);
    return database;
}

Instance makeInstance(int features_length) {
    Instance instance = (Instance) malloc(sizeof(S_Instance));
    instance->cluster_id = -1;
    instance->features = malloc(features_length * sizeof(double));
    return instance;
}

void printInstance(Instance instance, int features_length) {
    printf("%s,%d", instance->clazz, instance->cluster_id);
    int i;
    for (i = 0; i < features_length; i++) {
        printf(",%lf", instance->features[i]);
    }
    printf("\n");
}

void printDatabase(Database database) {
    int used = database->instances->used;
    printf("%s\n", database->path_database);
    printf("features_length -> %d\n", database->features_length);
    printf("number of instances -> %d\n", used);
    int i;
    for (i = 0; i < used; i++) {
        printInstance((Instance) database->instances->data[i], database->features_length);
    }
    printf("--------------------------------------\n");
}

void freeInstanceGeneric(void* instanceGeneric) {
    Instance instance = (Instance) instanceGeneric;
    freeInstance(instance);
}

void freeInstance(Instance instance) {
    if (!instance) 
        return; 
    if (instance->clazz)
        free(instance->clazz);
    if (instance->features)
        free(instance->features);
    free(instance);
}


void freeDatabase(Database database) {
    deepFreeArray(database->instances, freeInstanceGeneric);
    free(database->path_database);
    free(database);
}