#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <unistd.h>
#include "mpi.h"
#include "database_bin_handler_mpi.h"

Database readBinDatabase(const char* path_database, int features_length) {
    int nprocs;
    int rank;
    int begin;
    int end;
    int size_instance = sizeof(int) + (features_length * sizeof(double));
    int qty_instances;
    int part_size;
    int file_open_error;
    int i = 0;
    Database database;
    MPI_File fh;
    MPI_Offset offset;
    MPI_Offset total_number_of_bytes;

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    file_open_error = MPI_File_open(MPI_COMM_WORLD, path_database, 
            MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (file_open_error != MPI_SUCCESS) {
        char error_string[BUFSIZ];
        int length_of_error_string, error_class;
    
        MPI_Error_class(file_open_error, &error_class);
        MPI_Error_string(error_class, error_string, &length_of_error_string);
        printf("%3d: %s\n", rank, error_string);
    
        MPI_Error_string(file_open_error, error_string, &length_of_error_string);
        printf("%3d: %s\n", rank, error_string);
    
        MPI_Abort(MPI_COMM_WORLD, file_open_error);
        printf("Erro! Não foi possível abrir o arquivo '%s'.\n", path_database);
        exit(0);
    }

    MPI_File_get_size(fh, &total_number_of_bytes);
    qty_instances = total_number_of_bytes / size_instance;
    part_size = qty_instances / nprocs;
    begin =  part_size * rank;
    end = begin + part_size;
    if (rank == nprocs - 1) {
        end = qty_instances;
    }
    database = makeDatabase(path_database, (end - begin) + 2);
    database->features_length = features_length;
    offset = (MPI_Offset) begin * (sizeof(int) + (sizeof(double) * features_length));
    MPI_File_seek(fh, offset, MPI_SEEK_SET);
    MPI_Barrier(MPI_COMM_WORLD);

    while (begin < end) {
        Instance newInstance = makeInstance(features_length);
        database->instances->data[i++] = (void*) newInstance;
        MPI_File_read(fh, &(newInstance->clazz_code), 1, MPI_INT, MPI_STATUS_IGNORE);
        MPI_File_read(fh, newInstance->features, features_length, MPI_DOUBLE, MPI_STATUS_IGNORE);
        //printInstance(newInstance, database->features_length);
        begin++;
    }
    database->instances->used = i;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_File_close(&fh);
    
    return database;
}
