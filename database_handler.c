#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include "database_handler.h"

Database readDatabase(const char* path_database, int class_column) {
    Database database = makeDatabase(path_database, 100);
    FILE* file = fopen(path_database, "r");
    if (file == NULL) {
        printf("Erro! Não foi possível abrir o arquivo '%s'.\n", path_database);
        exit(0);
    }
    char line[MY_LINE_MAX];
    if (!fgets(line, sizeof line, file)) {
        return database;
    }
    setFeaturesLenght(line, database);
    int cont = 1;
    do {
        Instance newInstance = readInstance(line, class_column, database->features_length);
        addArray(database->instances, (void*) newInstance);
        cont++;
    } while (fgets(line, sizeof line, file));
    fclose(file);
    return database;
}

void setFeaturesLenght(char* line, Database database) {
    char* lineCp = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(lineCp, line);
    char* token = strtok(lineCp, ",");
    token = strtok(NULL, ",");
    int index = 0;
    while (token) {
        token = strtok(NULL, ",");
        index++;
    }
    database->features_length = index;
}

Instance readInstance(char* line, int class_column, int features_length) {
    Instance instance = makeInstance(features_length);
    int line_length = strlen(line);
    if (line[line_length - 1] == '\n') {
        line[line_length - 1] = '\0';
    }
    char* token = strtok(line, ",");
    int index = 0;
    int class_count = 0;
    while (token) {
        if (index == class_column) {
            instance->clazz = malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(instance->clazz, token);
            class_count++;
        } else {
            sscanf(token, "%lf", &(instance->features[index - class_count]));
        }
        token = strtok(NULL, ",");
        index++;
    }
    return instance;
}





/**
 * Testes em relação a leitura das base de dados
 */

// #define IRIS_DATABASE "/home/carlos/kmean_mpi/databases/iris/iris.data"
// #define IRIS_CLASS_COLUMN 4
// #define IMAGE_DATABASE  "/home/carlos/kmean_mpi/databases/image/segmentation.data"
// #define IMAGE_CLASS_COLUMN 0
// #define LETTER_RECOGNITION_DATABASE  "/home/carlos/kmean_mpi/databases/letter-recognition/letter-recognition.data"
// #define LETTER_RECOGNITION_CLASS_COLUMN 0

// int main(int argc, char **argv)
// {
//     int TOTAL_DATABASE = 10000;
//     int SLEEP_TIME = 7;
//     printf("My process ID : %d\n", getpid());
//     printf("My parent's ID: %d\n\n", getppid());
//     sleep(SLEEP_TIME);
//     for (int i = 0; i < 10; i++) {
//         Database* database = (Database*) malloc(sizeof(Database) * TOTAL_DATABASE);
//         for (int i = 0; i < TOTAL_DATABASE; i++) {
//             database[i] = readDatabase(argv[1], argv[2]);
//         }
//         printf("VAI LIBERAR a MEMÓRIA EM %d SEGUNDOS\n", SLEEP_TIME);
//         // sleep(SLEEP_TIME);
//         // for (int i = 0; i < TOTAL_DATABASE; i++) {
//         //     freeDatabase(database[i]);
//         // }
//         //free(database);
//     }
//     printf("MEMÓRIA LIBERADA\n");
//     sleep(SLEEP_TIME * 3);
//     return 0;
// }


