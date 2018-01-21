#ifndef _BIN_DATABASE_HANDLER_
#define _BIN_DATABASE_HANDLER_

#include <mpi.h>
#include <database/database_struct.h>

/**
 * Número de instâncias da última base de dados lida pelo método readBinDatabaseMPI.
 * */
int num_instances_g;

/**
 * Realiza a leitura de uma base de dados binária de forma paralela com MPI.
 * 
 * Uma base de dados binária deve estar formatada da seguinte forma para que 
 * a leitura seja realizada com sucesso: cada instância deve conter um <int>
 * e os próximos elementos devem ser do tipo <double> e representam as 
 * features da instância. O <int> representa um código da classe da instância
 * se a base de dados for classificada, caso contrário, deve conter a valor -1. 
 * 
 * @param path_database caminho da base de dados binária
 * @param features_length quantidade de features da base de dados
 * 
 * @return parte da base de dados que o processo leu do arquivo binário
 * */
Database BinDatabaseReader_ReadMPI(const char* database_path, int features_length);

/**
 * Calcula e define o deslocamento que o processo deverá realizar no arquivo.
 * 
 * @param fh pointeiro para o arquivo, que está sendo lido com MPI
 * @param features_length quantidade de features da base de dados
 * 
 * @return quantidade de elementos que esse processo deverá ler da base de dados.
 * */
int BinDatabaseReader_SetOffsetFileMPI(MPI_File* fh, int features_length);

/**
 * Conta e retorna a quantidade de instâncias que contém em uma determinada base
 * de dados binária.
 * 
 * @param fh pointeiro para o arquivo, que está sendo lido com MPI
 * @param instance_size tamanho em bytes de uma instância da base de dados
 * 
 * @return quantidade de instâncias da base de dados
 * */
int BinDatabaseReader_CountInstancesMPI(MPI_File* fh, int instance_size);

/**
 * Realiza um log de um deterterminado erro ao tentar abrir um arquivo com MPI e 
 * finaliza a aplicação.
 * 
 * @param file_open_error código do erro que ocorreu ao tentar abrir o arquivo com MPI
 * @param file_path caminho do arquivo que deu erro ao tentar abrir
 * */
void BinDatabaseReader_LogFileOpenErrorMPI(int file_open_error, const char* file_path);

#endif /*_BIN_DATABASE_HANDLER_*/