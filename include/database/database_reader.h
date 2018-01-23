#ifndef _DATABASE_HANDLER_
#define _DATABASE_HANDLER_

#include <database/database_struct.h>

/**
 * Tamanho do buffer que irá ler as linhas do arquivo da base de dados.
 * */
#define MY_LINE_MAX 4000

/**
 * Realiza a leitura de uma base de dados em csv.
 * 
 * O arquivo da base de dados não pode conter header, cada linha do arquivo
 * deve conter uma instância da base de dados. Exceto a coluna da classe, todas
 * outras colunas (features) devem conter valores numéricos.
 * ATUALMENTE NÃO SUPORTA VALORES AUSENTES.
 * 
 * @param database_path
 *      caminho da base de dados em csv
 * @param class_column
 *      índice da coluna que contém a classe das instâncias, caso
 *      a base de dados não seja classificada, esse índice deve ser -1
 * 
 * @return base de dados extraída do arquivo csv
 * */
Database DatabaseReader_ReadDatabase(const char* database_path, int class_column);

/**
 * Define a quantidade de features dos elementos de uma base de dados com base em um
 * elemento dessa base de dados.
 * 
 * @param line
 *      linha que representa uma instância da base de dados
 * @param database
 *      base de dados que terá sua quantidade de features definida
 * */
void DatabaseReader_SetFeaturesLenght(char* line, Database database);

/**
 * Extraí uma instância da base de dados a partir de uma determinada linha 
 * que a representa.
 * 
 * @param line linha que representa a instância da base de dados
 * @param class_column 
 *      índice da coluna que contém a classe da instância, caso
 *      a base de dados não seja classificada, esse índice deve ser -1
 * @param features_length
 *      quantidade de features da base de dados
 * 
 * @return instância extraída da linha da base de dados
 * */
Instance DatabaseReader_ReadInstance(char* line, int class_column, int features_length);

#endif /*_DATABASE_HANDLER_*/