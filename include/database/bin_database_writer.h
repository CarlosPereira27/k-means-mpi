#include <database/database_struct.h>
#include <array/array.h>

/**
 * Extensão do arquivo da base de dados binária.
 * */
#define BIN_EXTENSION ".bin"
/**
 * Tamanho da string (+1) que representa a extensão do arquivo da base de dados binária.
 * */
#define BIN_EXTENSION_LENGTH 5
/**
 * Extensão do arquivo que contém as clazzes e seus respectivos códigos 
 * de uma base de dados. 
 * */
#define CLASS_EXTENSION "_class.csv"
/**
 * Tamanho da string (+1) que representa a extensão do arquivo que 
 * contém as classes e seus respectivos códigos de uma base de dados. 
 * */
#define CLASS_EXTENSION_LENGTH 11


/**
 * Recupera o nome da base de dados binária de uma determinada base de dados.
 * 
 * @param database_name nome da base de dados original
 * 
 * @return nome da base de dados binária
 * */
char* BinDatabaseWriter_GetBinDatabaseName(const char* database_name);

/**
 * Recupera o nome da base de dados das classes de uma determinada base de dados.
 * 
 * @param database_name nome da base de dados original
 * 
 * @return nome da base de dados das classes 
 * */
char* BinDatabaseWriter_GetClassDatabaseName(const char* database_name);

/**
 * Recupera o código de uma classe de uma lista de classes. O código da classe
 * é a sua posição na lista. Caso uma classe ainda não esteja contida na lista,
 * ela é inserida na lista e seu código é a posição em que foi inserida.
 * 
 * @param classes lista que contém as classes já encontradas
 * @param class_name  nome da classe a ser recuperado o código
 * 
 * @return código da classe
 * */
int BinDatabaseWriter_GetClassCod(ArrayList classes, const char* class_name);

/**
 * Escreve a base de dados das classes no arquivo.
 * 
 * @param database_name nome da base dados original
 * @param classes lista que contém as classes da base de dados
 * */
void BinDatabaseWriter_WriteClassDatabase(char* database_name, ArrayList classes);

/**
 * Escreve a base de dados binária no arquivo.
 * 
 * @param database_name nome da base dados a ser escrita de forma binária
 * @param classes lista que contém as classes da base de dados
 * @param database base de dados a ser escrita de forma binária
 * */
void BinDatabaseWriter_WriteBinDatabase(char* database_name, ArrayList classes, Database database);