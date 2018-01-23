#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <database/bin_database_writer.h>
#include <database/database_reader.h>

/**
 * Recupera o nome da base de dados binária de uma determinada base de dados.
 * 
 * @param database_name
 *      nome da base de dados original
 * 
 * @return nome da base de dados binária
 * */
char* BinDatabaseWriter_GetBinDatabaseName(const char* database_name) {
    int len_name = strlen(database_name);
    char* bin_name = (char*) malloc((len_name + BIN_EXTENSION_LENGTH) * sizeof(char));
    
    strcpy(bin_name, database_name);
    strcpy(&(bin_name[len_name]), BIN_EXTENSION);
    
    return bin_name;
}

/**
 * Recupera o nome da base de dados das classes de uma determinada base de dados.
 * 
 * @param database_name
 *      nome da base de dados original
 * 
 * @return nome da base de dados das classes 
 * */
char* BinDatabaseWriter_GetClassDatabaseName(const char* database_name) {
    int len_name = strlen(database_name);
    char* class_name = (char*) malloc((len_name + CLASS_EXTENSION_LENGTH) * sizeof(char));
    
    strcpy(class_name, database_name);
    strcpy(&(class_name[len_name]), CLASS_EXTENSION);

    return class_name;
}

/**
 * Recupera o código de uma classe de uma lista de classes. O código da classe
 * é a sua posição na lista. Caso uma classe ainda não esteja contida na lista,
 * ela é inserida na lista e seu código é a posição em que foi inserida.
 * 
 * @param classes
 *      lista que contém as classes já encontradas
 * @param class_name
 *      nome da classe a ser recuperado o código
 * 
 * @return código da classe
 * */
int BinDatabaseWriter_GetClassCod(ArrayList classes, const char* class_name) {
    int i = 0;
    int N = classes->used;

    for (i = 0; i < N; i++) {
        const char* class_name_i = (char*) classes->data[i];
        if (strcmp(class_name_i, class_name) == 0) {
            return i;
        }
    }
    ArrayList_Add(classes, (void*) class_name);

    return N;
}

/**
 * Escreve a base de dados das classes no arquivo.
 * 
 * @param database_name
 *      nome da base dados original
 * @param classes
 *      lista que contém as classes da base de dados
 * */
void BinDatabaseWriter_WriteClassDatabase(char* database_name, ArrayList classes) {
    char* class_database_name = BinDatabaseWriter_GetClassDatabaseName(database_name);
    int i;
    int N = classes->used;
    FILE* file = fopen(class_database_name, "w+");

    if (file == NULL) {
        printf("Erro! Não foi possível criar o arquivo '%s'.\n", class_database_name);
        exit(0);
    }
    fprintf(file, "class name,class code\n");
    for (i = 0; i < N; i++) {
        const char* class_name = (char*) classes->data[i];
        fprintf(file, "%s,%d\n", class_name, i);
    }

    fclose(file);
    printf("Os dados das classes da base de dados '%s' ", database_name);
    printf("foram salvos com sucesso no arquivo '%s'.\n", class_database_name);
}

/**
 * Escreve a base de dados binária no arquivo.
 * 
 * @param database_name
 *      nome da base dados a ser escrita de forma binária
 * @param classes
 *      lista que contém as classes da base de dados
 * @param database
 *      base de dados a ser escrita de forma binária
 * */
void BinDatabaseWriter_WriteBinDatabase(char* database_name, ArrayList classes, Database database) {
    char* bin_database_name = BinDatabaseWriter_GetBinDatabaseName(database_name);
    int N = database->instances->used;
    int features_length = database->features_length;
    int i;
    FILE* file = fopen(bin_database_name, "w");
    void** instances = database->instances->data;

    if (file == NULL) {
        printf("Erro! Não foi possível criar o arquivo '%s'.\n", bin_database_name);
        exit(0);
    }
    printf("features_length -> %d\n", features_length);
    printf("N -> %d\n", N);
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        int class_code = BinDatabaseWriter_GetClassCod(classes, instance->clazz);
        fwrite(&class_code, sizeof(int), 1, file);
        fwrite(instance->features, sizeof(double), features_length, file);
    }

    fclose(file);
    printf("Os dados binários da base de dados '%s' ", database_name);
    printf("foram salvos com sucesso no arquivo '%s'.\n", bin_database_name);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("A quantidade de argumentos é inválida!\n");
        exit(0);
    }
    Database database = DatabaseReader_ReadDatabase(argv[1], atoi(argv[2]));
    ArrayList classes = ArrayList_Create(20);
    
    BinDatabaseWriter_WriteBinDatabase(argv[1], classes, database);
    BinDatabaseWriter_WriteClassDatabase(argv[1], classes);
    
    return 0;
} 