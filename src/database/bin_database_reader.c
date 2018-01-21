#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <unistd.h>
#include <mpi.h>
#include <database/bin_database_reader.h>


/**
 * Realiza a leitura de uma base de dados binária de forma paralela com MPI.
 * 
 * Uma base de dados binária deve estar formatada da seguinte forma para que 
 * a leitura seja realizada com sucesso: cada instância deve conter um <int>
 * e os próximos elementos devem ser do tipo <double> e representam as 
 * features da instância. O <int> representa um código da classe da instância
 * se a base de dados for classificada, caso contrário, deve conter a valor -1. 
 * 
 * @param database_path caminho da base de dados binária
 * @param features_length quantidade de features da base de dados
 * 
 * @return parte da base de dados que o processo leu do arquivo binário
 * */
Database BinDatabaseReader_ReadMPI(const char* database_path, int features_length) {
    int num_instances_proc;
    int file_open_error;
    int i = 0;
    Database database;
    MPI_File fh;
    
    file_open_error = MPI_File_open(MPI_COMM_WORLD, database_path, 
            MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (file_open_error != MPI_SUCCESS) {
        BinDatabaseReader_LogFileOpenErrorMPI(file_open_error, database_path);
    }

    num_instances_proc = BinDatabaseReader_SetOffsetFileMPI(&fh, features_length);
    database = Database_Create(database_path, num_instances_proc + 2);
    database->features_length = features_length;
    database->instances->used = num_instances_proc;

    for (i = 0; i < num_instances_proc; i++) {
        Instance newInstance = Instance_Create(features_length);
        database->instances->data[i] = (void*) newInstance;
        MPI_File_read(fh, &(newInstance->class_code), 1, MPI_INT, MPI_STATUS_IGNORE);
        MPI_File_read(fh, newInstance->features, features_length, 
                MPI_DOUBLE, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&fh);
    
    return database;
}

/**
 * Calcula e define o deslocamento que o processo deverá realizar no arquivo.
 * 
 * @param fh pointeiro para o arquivo, que está sendo lido com MPI
 * @param features_length quantidade de features da base de dados
 * 
 * @return quantidade de elementos que esse processo deverá ler da base de dados.
 * */
int BinDatabaseReader_SetOffsetFileMPI(MPI_File* fh, int features_length) {
    int rank;
    int nprocs;
    MPI_Offset offset;
    int size_instance = sizeof(int) + (features_length * sizeof(double));
    num_instances_g = BinDatabaseReader_CountInstancesMPI(fh, size_instance);
    int num_instances_proc;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    num_instances_proc = num_instances_g / nprocs;
    offset = (MPI_Offset) (num_instances_proc * rank) * size_instance;
    MPI_File_seek(*fh, offset, MPI_SEEK_SET);
    if (rank == nprocs - 1) {
        num_instances_proc += num_instances_g % nprocs;
    }

    return num_instances_proc;
}

/**
 * Conta e retorna a quantidade de instâncias que contém em uma determinada base
 * de dados binária.
 * 
 * @param fh pointeiro para o arquivo, que está sendo lido com MPI
 * @param instance_size tamanho em bytes de uma instância da base de dados
 * 
 * @return quantidade de instâncias da base de dados
 * */
int BinDatabaseReader_CountInstancesMPI(MPI_File* fh, int instance_size) {
    MPI_Offset num_bytes;
    
    MPI_File_get_size(*fh, &num_bytes);
    
    return num_bytes / instance_size;
}

/**
 * Realiza um log de um deterterminado erro ao tentar abrir um arquivo com MPI e 
 * finaliza a aplicação.
 * 
 * @param file_open_error código do erro que ocorreu ao tentar abrir o arquivo com MPI
 * @param file_path caminho do arquivo que deu erro ao tentar abrir
 * */
void BinDatabaseReader_LogFileOpenErrorMPI(int file_open_error, const char* file_path) {
    char error_string[BUFSIZ];
    int length_of_error_string;
    int error_class;
    int rank;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Error_class(file_open_error, &error_class);
    MPI_Error_string(error_class, error_string, &length_of_error_string);
    printf("%3d: %s\n", rank, error_string);

    MPI_Error_string(file_open_error, error_string, &length_of_error_string);
    printf("%3d: %s\n", rank, error_string);

    MPI_Abort(MPI_COMM_WORLD, file_open_error);
    printf("Erro! Não foi possível abrir o arquivo '%s'.\n", file_path);
    exit(0);
}