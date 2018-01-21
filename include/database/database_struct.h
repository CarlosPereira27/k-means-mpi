#ifndef _DATABASE_STRUCT_
#define _DATABASE_STRUCT_

#include <array/array.h>

/**
 * Representa uma instância de uma base de dados. Esta instância pode possuir uma classe,
 * caso a base de dados já for classificada, para representar esta instância é utilizado
 * uma string (clazz) e um inteiro (clazz_code) que representam o rótulo e o código da 
 * classe respectivamente. O array features representa as features da base de dados e o
 * inteiro cluster_id representa em que cluster a instância está situada, quando executado
 * um algoritmo de clusterização. 
 * */
typedef struct
{
    char* clazz;
    double* features;
    int cluster_id;
    int class_code;
} S_Instance;

/**
 * Representa uma base de dados. A lista instances contém as instâncias 
 * da base. A string path_database representa o caminho do arquivo que contém a base
 * de dados. O inteiro features_length representa quantas features as instâncias desta 
 * base de dados possuem.
 * */
typedef struct 
{
    char* database_path;
    ArrayList instances;
    int features_length;
} S_Database;

/**
 * Definição do tipo ponteiro de instância.
 * */
typedef S_Instance* Instance;

/**
 * Definição do tipo ponteiro de base de dados.
 * */
typedef S_Database* Database;

/**
 * Cria uma base de dados definindo o caminho do arquivo que a contém e o tamanho inicial
 * da lista das instâncias
 * 
 * @param database_path caminho do arquivo que contém a base de dados
 * @param length tamanho inicial da lista das instâncias
 * 
 * @return base de dados vazia criada
 * */
Database Database_Create(const char* database_path, int length);

/**
 * Cria uma instância com um array com uma determinada quantidade de features.
 * 
 * @param features_length quantidade de features da instância
 * 
 * @return instância criada
 * */
Instance Instance_Create(int features_length);

/**
 * Imprime uma instância na saída padrão.
 * 
 * @param instance instância a ser impressa na saída padrão
 * @param features_length quantidade de features da instância
 * */
void Instance_Print(Instance instance, int features_length);

/**
 * Imprime uma base de dados na saída padrão.
 * 
 * @param database base de dados a ser impressa na saída padrão
 * */
void Database_Print(Database database);

/**
 * Libera a memória de uma instância de forma genérica. Recebe um
 * pointeiro de void e realiza o cast para Instance para liberar a memória.
 * 
 * @param instanceGeneric instância a liberar memória como um ponteiro de void
 * */
void Instance_DestroyGeneric(void* instanceGeneric);

/**
 * Libera a memória de uma determinada instância.
 * 
 * @param instance instância a liberar memória
 * */
void Instance_Destroy(Instance instance);

/**
 * Libera a memória de uma base de dados.
 * 
 * @param database base de dados a liberar memória
 * */
void Database_Destroy(Database database);

#endif /*_DATABASE_STRUCT_*/