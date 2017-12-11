#include "mpi.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <time.h>
#include "database_bin_handler_mpi.h"
#include "kmeans_mpi.h"


int nproc;
int rank;
Database database;
double** centroids;

double* requestCentroid(int unsigned index) {
    if (index < database->instances->used) {
        return getCopyFeatures(index);
    }
    double* centroid = (double*) malloc(database->features_length * sizeof(double));
    int rank_request = index / database->instances->used;
    index %= database->instances->used;
    printf("REQUISITANDO centroid %d para processo %d\n", index, rank_request);
    MPI_Send(&index, 1, MPI_INT, rank_request, TAG, MPI_COMM_WORLD);
    printf("ESPERANDO centroid %d do processo %d\n", index, rank_request);
    MPI_Recv(centroid, database->features_length, MPI_DOUBLE, rank_request, 
        TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("RECEBIDO centroid %d do processo %d\n", index, rank_request);
    return centroid;
}

void respondCentroid(int index) {
    Instance instance = (Instance) database->instances->data[index];
    printf("ENVIANDO centroid %d para mestre %d de worker %d\n", index, MASTER_RANK, rank);
    MPI_Send(instance->features, database->features_length, MPI_DOUBLE, 
        MASTER_RANK, TAG, MPI_COMM_WORLD);
}

double* getCopyFeatures(int index) {
    double* cp_features = (double*) malloc(database->features_length * sizeof(double));
    int i = 0;
    Instance instance = database->instances->data[index];
    while (i < database->features_length) {
        cp_features[i] = instance->features[i];
        i++;
    }
    return cp_features;
}

BOOLEAN containsValue(int* array, int length, int value) {
    int i = 0;
    while (i < length) {
        if (array[i] == value) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
} 

int* randomKIndexes(int K) {
    int* indexes = (int*) malloc(K * sizeof(int));
    int i = 0;
    while (i < K) {
        int value = rand() % (database->instances->used * nproc);
        while (containsValue(indexes, i, value)) {
            value = rand() % (database->instances->used * nproc);
        }
        indexes[i] = value;
        i++;
    }
    return indexes;
}

void centroidsComputed() {
    int i = 0;
    int centroidsComputedValue = -1;
    while (i < nproc) {
        if (i != MASTER_RANK) {
            MPI_Send(&centroidsComputedValue, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }
        i++;
    }
}

void compStartCentroidsMPI(int K) {
    int i;

    centroids = (double**) malloc(K * sizeof(double*));
    for (i = 0; i < K; i++) {
        centroids[i] = (double*) malloc(database->features_length * sizeof(double));
    }
    if (rank == MASTER_RANK) {
        printf("SOU MESTRE\n");
        int* indexes = randomKIndexes(K);
        for (i = 0; i < K; i++) {
            centroids[i] = requestCentroid(indexes[i]);
        }
        free(indexes);
        centroidsComputed();
    } else {
        int msg;
        printf("ESPERANDO MSG %d\n", rank);
        MPI_Recv(&msg, 1, MPI_INT, MASTER_RANK, 
            TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("msg -> %d, recebido do mestre %d no processo%d\n", msg, MASTER_RANK, rank);
        while (msg != -1) {
            respondCentroid(msg);
            MPI_Recv(&msg, 1, MPI_INT, MASTER_RANK, 
                TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    for (i = 0; i < K; i++) {
        MPI_Bcast(centroids[i], database->features_length, MPI_DOUBLE, 
            MASTER_RANK, MPI_COMM_WORLD);
    }
}

double difQuadNormVectMPI(double* vect1, double* vect2, int N) {
    int i = 0;
    double difNorm = 0.0;
    for (i = 0; i < N; i++) {
        double dif = vect1[i] - vect2[i];
        difNorm += dif * dif;
    }
    return difNorm;
}



double compJMPI(int K) {
    double bestClusterNorm, normVect;
    int N = database->instances->used;
    int i = 0, k;
    double J = 0.0;
    void** instances = database->instances->data;
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        k = 0;
        bestClusterNorm = difQuadNormVectMPI(instance->features, centroids[k], 
                database->features_length);
        instance->cluster_id = k;

        for (k = 1; k < K; k++) {
            normVect = difQuadNormVectMPI(instance->features, centroids[k], 
                database->features_length);
            if (normVect < bestClusterNorm) {
                bestClusterNorm = normVect;
                instance->cluster_id = k;
            }
        }
        J += bestClusterNorm;
    }
    return J;
}


void recalcClusterCentroidMPI(int K) {
    printf("ATE AQUI FOI(0) %d\n", rank);
    int clusterCount[K];
    int i, j;
    for (i = 0; i < K; i++) {
        clusterCount[i] = 0;
        for (j = 0; j < database->features_length; j++) {
            centroids[i][j] = 0.0;
        }
    }
    printf("ATE AQUI FOI(1) %d\n", rank);
    int N = database->instances->used;
    void** instances = database->instances->data;
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        clusterCount[instance->cluster_id]++;
        for (j = 0; j < database->features_length; j++) {
            centroids[instance->cluster_id][j] += instance->features[j];
        }
    }
    printf("ATE AQUI FOI %d\n", rank);
    int* cluster_count_sum;
    double** centroids_sum;

    if (rank == MASTER_RANK) {
        cluster_count_sum = (int*) malloc(K * sizeof(int));
        centroids_sum = (double**) malloc(K * sizeof(double*));
        for (i = 0; i < K; i++) {
            cluster_count_sum[i] = 0;
            centroids_sum[i] = (double*) malloc(database->features_length * sizeof(double*));
            for (j = 0; j < database->features_length; j++) {
                centroids_sum[i][j] = 0.0;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(clusterCount, cluster_count_sum, K, MPI_INT, MPI_SUM, 
        MASTER_RANK, MPI_COMM_WORLD);
    for (i = 0; i < K; i++) {
        MPI_Reduce(centroids[i], centroids_sum[i], K, MPI_DOUBLE, MPI_SUM, 
            MASTER_RANK, MPI_COMM_WORLD);
    }
    printf("ATE AQUI FOI FA %d\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == MASTER_RANK) {
        for (i = 0; i < K; i++) {
            printf("pointer -> %p\n", cluster_count_sum);
            printf("cluster_cout -> %d\n", cluster_count_sum[i]);
            printPointMPI(cluster_count_sum, K);
            printf("\n");
            printListPointsMPI(centroids_sum, K, database->features_length);
            if (cluster_count_sum[i]) {
                for (j = 0; j < database->features_length; j++) {
                    centroids[i][j] = centroids_sum[i][j] / cluster_count_sum[i];
                }
            } else {

            }
        }

    }
     printf("ATE AQUI FOI LAST %d\n", rank);
    MPI_Bcast(centroids[i], database->features_length, MPI_DOUBLE, 
            MASTER_RANK, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
}

void printListPointsMPI(double** listPoint, int length1, int length2) {
    int i;
    for (i = 0; i < length1; i++) {
        printf("\nPoint %d:\n", i);
        printPointMPI(listPoint[i], length2);
    }
    printf("\n-----------------------\n");
}

void printPointMPI(double* point, int length) {
    int i = 0;
    printf("%lf", point[i]);
    for (i++; i < length; i++) {
        printf(", %lf", point[i]);
    }
}



void writeInstanceMPI(Instance instance, int features_length, MPI_File file) {
    char buffer[1000];
    int len;
    int i;

    for (i = 0; i < features_length; i++) {
        sprintf(buffer, "%lf,", instance->features[i]);
        len = strlen(buffer);
        MPI_File_write(file, buffer, len, MPI_CHAR, MPI_STATUS_IGNORE);
    }
    sprintf(buffer, "%d\n", instance->clazz_code);
    len = strlen(buffer);
    MPI_File_write(file, buffer, len, MPI_CHAR, MPI_STATUS_IGNORE);
    // sprintf(buffer, "\n");
    // len = strlen(buffer);
    // MPI_File_write(file, buffer, len, MPI_CHAR, MPI_STATUS_IGNORE);
}

void writeInstance(Instance instance, int features_length, FILE* file) {
    fprintf(file, "%d", instance->clazz_code);
    int i;
    for (i = 0; i < features_length; i++) {
        fprintf(file, ",%lf", instance->features[i]);
    }
    fprintf(file, "\n");
}

int runKmeansMPI(int argc, char **argv) {
    srand(time(NULL));   // should only be called once
    if (argc != 4) {
        printf("A quantidade de argumentos é inválida!\n");
        exit(0);
    }
    int K = atoi(argv[3]);
    database = readBinDatabase(argv[1], atoi(argv[2]));
    // if (rank == 0) {
    //     printDatabase(database);
    // }
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("READ SUCESSS\n");
    //printf("Size database -> %d (rank: %d)\n", database->instances->used, rank);
    //printDatabase(database);
    compStartCentroidsMPI(K);
    printf("CENTROID SUCCESS\n");
    //printf("Primeiros centroids:\n");
    //printListPointsMPI(clustersCentroid, K, database->features_length);
    double JLineLocal = compJMPI(K);
    double JLine = 0;
    MPI_Reduce(&JLineLocal, &JLine, 1, MPI_DOUBLE, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(&JLine, 1, MPI_DOUBLE, 
            MASTER_RANK, MPI_COMM_WORLD);
    //printf("J -> %lf\n", JLine);
    double J;
    int countIt = 0;
    do {
        J = JLine;
        recalcClusterCentroidMPI(K);
        printf("SUCCESS\n");
        JLineLocal = compJMPI(K);
        JLine = 0;
        MPI_Reduce(&JLineLocal, &JLine, 1, MPI_DOUBLE, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        MPI_Bcast(&JLine, 1, MPI_DOUBLE, 
            MASTER_RANK, MPI_COMM_WORLD);
        countIt++;
       // printf("J -> %lf\n", J);
    } while (J - JLine > THRESHOLD_MPI);
    if (rank == MASTER_RANK) {
        printf("QTD iteracoes: %d\n", countIt);
        printf("Centroids finais:\n");
        printListPointsMPI(centroids, K, database->features_length);
    }
    // char* outFile = (char*) malloc(20 * sizeof(char));
    // strcpy(outFile, "teste.out");
    // //sprintf(&(outFile[4]), "%d", rank);

    // int rank;
    // int nproc;
    // int len;
    // int size_instance = sizeof(int) + (features_length * sizeof(double));
    // int qty_instances;
    // int part_size;

    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    // printf("Rank(%d) LEITURA REALIZADA!\n", rank);
    // // WRITE WITH MPI_FILE
    // MPI_File fh;
    // int file_open_error = MPI_File_open(MPI_COMM_WORLD, outFile, 
    //         MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
    // if (file_open_error != MPI_SUCCESS) {
    //     char error_string[BUFSIZ];
    //     int length_of_error_string, error_class;
    
    //     MPI_Error_class(file_open_error, &error_class);
    //     MPI_Error_string(error_class, error_string, &length_of_error_string);
    //     printf("%3d: %s\n", rank, error_string);
    
    //     MPI_Error_string(file_open_error, error_string, &length_of_error_string);
    //     printf("%3d: %s\n", rank, error_string);
    //     printf("Erro! Não foi possível abrir o arquivo '%s'.\n", outFile);
    //     MPI_Abort(MPI_COMM_WORLD, file_open_error);
        
    //     exit(0);
    // }
    // MPI_File_get_size(fh, &total_number_of_bytes);
    // qty_instances = total_number_of_bytes / size_instance;
    // part_size = qty_instances / nprocs;
    // begin =  part_size * rank;
    // end = begin + part_size;
    // if (rank == nprocs - 1) {
    //     end = qty_instances;
    // }
    // offset = (MPI_Offset) begin * (sizeof(int) + (sizeof(double) * features_length));
    // MPI_File_seek(fh, offset, MPI_SEEK_SET);
    // int used = database->instances->used;
    // int i;
    // for (i = 0; i < used; i++) {
    //     writeInstanceMPI((Instance) database->instances->data[i], database->features_length, fh);
    // }
    
    // MPI_File_close(&fh);

    // WRITE WITH FILE
    // FILE* file = fopen(outFile, "w+");
    // if (file == NULL) {
    //     printf("Erro! Rank (%d) não foi possível abrir o arquivo %s!\n", rank, outFile);
    //     exit(0);
    // }
    // int used = database->instances->used;
    // int i;
    // for (i = 0; i < used; i++) {
    //     writeInstance((Instance) database->instances->data[i], database->features_length, file);
    // }
    // fclose(file);
    return 0;
} 