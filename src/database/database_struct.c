#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <database/database_struct.h>

/**
 * Cria uma base de dados definindo o caminho do arquivo que a contém e o tamanho inicial
 * da lista das instâncias
 * 
 * @param database_path
 *      caminho do arquivo que contém a base de dados
 * @param length
 *      tamanho inicial da lista das instâncias
 * 
 * @return base de dados vazia criada
 * */
Database Database_Create(const char* database_path, int length) {
    Database database = (Database) malloc(sizeof(S_Database));
    int name_length = strlen(database_path) + 1;

    database->database_path = (char*) malloc(sizeof(char) * name_length);
    strcpy(database->database_path, database_path);
    database->instances = ArrayList_Create(length);

    return database;
}

/**
 * Cria uma instância com um array com uma determinada quantidade de features.
 * 
 * @param features_length
 *      quantidade de features da instância
 * 
 * @return instância criada
 * */
Instance Instance_Create(int features_length) {
    Instance instance = (Instance) malloc(sizeof(S_Instance));

    instance->cluster_id = -1;
    instance->features = malloc(features_length * sizeof(double));

    return instance;
}

/**
 * Imprime uma instância na saída padrão.
 * 
 * @param instance
 *      instância a ser impressa na saída padrão
 * @param features_length
 *      quantidade de features da instância
 * */
void Instance_Print(Instance instance, int features_length) {
    printf("%s,%d", instance->clazz, instance->cluster_id);
    int i;
    for (i = 0; i < features_length; i++) {
        printf(",%lf", instance->features[i]);
    }
    printf("\n");
}

/**
 * Imprime uma base de dados na saída padrão.
 * 
 * @param database
 *      base de dados a ser impressa na saída padrão
 * */
void Database_Print(Database database) {
    int used = database->instances->used;
    int i;
    
    printf("%s\n", database->database_path);
    printf("features_length -> %d\n", database->features_length);
    printf("number of instances -> %d\n", used);
    for (i = 0; i < used; i++) {
        Instance_Print((Instance) database->instances->data[i], database->features_length);
    }
    printf("--------------------------------------\n");
}

/**
 * Libera a memória de uma instância de forma genérica. Recebe um
 * pointeiro de void e realiza o cast para Instance para liberar a memória.
 * 
 * @param instance_generic
 *      instância a liberar memória como um ponteiro de void
 * */
void Instance_DestroyGeneric(void* instance_generic) {
    Instance instance = (Instance) instance_generic;
    Instance_Destroy(instance);
}

/**
 * Libera a memória de uma determinada instância.
 * 
 * @param instance
 *      instância a liberar memória
 * */
void Instance_Destroy(Instance instance) {
    if (instance == NULL)
        return; 
    if (instance->clazz != NULL)
        free(instance->clazz);
    if (instance->features != NULL)
        free(instance->features);
    free(instance);
}

/**
 * Libera a memória de uma base de dados.
 * 
 * @param database
 *      base de dados a liberar memória
 * */
void Database_Destroy(Database database) {
    if (database == NULL)
        return;
    ArrayList_DestroyDeeply(database->instances, Instance_DestroyGeneric);
    if (database->database_path != NULL)
        free(database->database_path);
    free(database);
}