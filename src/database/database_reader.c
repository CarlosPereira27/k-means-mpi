#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <database/database_reader.h>

/**
 * Realiza a leitura de uma base de dados em csv.
 * 
 * O arquivo da base de dados não pode conter header, cada linha do arquivo
 * deve conter uma instância da base de dados. Exceto a coluna da classe, todas
 * outras colunas (features) devem conter valores numéricos.
 * ATUALMENTE NÃO SUPORTA VALORES AUSENTES.
 * 
 * @param database_path caminho da base de dados em csv
 * @param class_column índice da coluna que contém a classe das instâncias, caso
 *      a base de dados não seja classificada, esse índice deve ser -1
 * 
 * @return base de dados extraída do arquivo csv
 * */
Database DatabaseReader_ReadDatabase(const char* database_path, int class_column) {
    Database database = Database_Create(database_path, 100);
    FILE* file = fopen(database_path, "r");
    char line[MY_LINE_MAX];
    int cont = 1;

    if (file == NULL) {
        printf("Erro! Não foi possível abrir o arquivo '%s'.\n", database_path);
        exit(0);
    }
    if (!fgets(line, sizeof line, file)) {
        return database;
    }
    DatabaseReader_SetFeaturesLenght(line, database);
    do {
        Instance new_instance = DatabaseReader_ReadInstance(line, class_column, database->features_length);
        ArrayList_Add(database->instances, (void*) new_instance);
        cont++;
    } while (fgets(line, sizeof line, file));
    fclose(file);
    
    return database;
}

/**
 * Define a quantidade de features dos elementos de uma base de dados com base em um
 * elemento dessa base de dados.
 * 
 * @param line linha que representa uma instância da base de dados
 * @param database base de dados que terá sua quantidade de features definida
 * */
void DatabaseReader_SetFeaturesLenght(char* line, Database database) {
    char* line_cp = malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(line_cp, line);
    char* token = strtok(line_cp, ",");
    int index = 0;

    token = strtok(NULL, ",");
    while (token) {
        token = strtok(NULL, ",");
        index++;
    }
    database->features_length = index;
}

/**
 * Extraí uma instância da base de dados a partir de uma determinada linha 
 * que a representa.
 * 
 * @param line linha que representa a instância da base de dados
 * @param class_column índice da coluna que contém a classe da instância, caso
 *      a base de dados não seja classificada, esse índice deve ser -1
 * @param features_length quantidade de features da base de dados
 * 
 * @return instância extraída da linha da base de dados
 * */
Instance DatabaseReader_ReadInstance(char* line, int class_column, int features_length) {
    Instance instance = Instance_Create(features_length);
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