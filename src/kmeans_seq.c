
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <database/database_reader.h>
#include <kmeans_seq.h>



/**
 * Índice da próxima instância a ser requisitada pelo método determinístico.
 * */
int next_instance_deterministic = 0;

/**
 * 
 * */
double** KMeansSeq_CompStartCentroidsX(int K) {
    double** startCentroids = (double**) malloc(K * sizeof(double*));
    double minValues[database->features_length];
    double maxValues[database->features_length];
    int N = database->instances->used;
    int i = 0, j;
    void** instances = database->instances->data;
    for (j = 0; j < database->features_length; j++) {
        Instance instance = (Instance) instances[i];
        minValues[j] = maxValues[j] = instance->features[j];  
    }
    for (i = 1; i < N; i++) {
        Instance instance = (Instance) instances[i];
        for (j = 0; j < database->features_length; j++) {
            if (instance->features[j] < minValues[j]) {
                minValues[j] = instance->features[j];
            }
            if (instance->features[j] > maxValues[j]) {
                maxValues[j] = instance->features[j];
            }
        }
    }
    // printf("\nMinValues:\n");
    // printPoint(minValues, database->features_length);
    // printf("\nMaxValues:\n");
    // printPoint(maxValues, database->features_length);
    // printf("\n--------------------------------\n");
    double offset[database->features_length];
    for (j = 0; j < database->features_length; j++) {
        offset[j] = (maxValues[j] - minValues[j]) / (K + 1);
    }
    for (i = 0; i < K; i++) {
        startCentroids[i] = (double*) malloc(database->features_length * sizeof(double));
        for (j = 0; j < database->features_length; j++) {
            startCentroids[i][j] = minValues[j] + (offset[j] * (i + 1));
        }
    }
    return startCentroids;
}


/**
 * Recalcula os centroids
 * 
 * */
void KMeansSeq_RecalcClusterCentroids(int K) {
    int cluster_count[K];
    int i, j;
    memset(cluster_count, 0, K * sizeof(int));
    Util_MemsetMatrix((void**) centroids, 0, K, database->features_length * sizeof(double));
    int N = database->instances->used;
    void** instances = database->instances->data;
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        cluster_count[instance->cluster_id]++;
        for (j = 0; j < database->features_length; j++) {
            centroids[instance->cluster_id][j] += instance->features[j];
        }
    }  
    for (i = 0; i < K; i++) {
        if (cluster_count[i] == 0) {
            centroids[i] = KMeans_GetCopyFeatures(next_instance_deterministic);
            next_instance_deterministic = 
                (next_instance_deterministic + 1) % database->instances->used;
            continue;            
        }
        for (j = 0; j < database->features_length; j++) {
            centroids[i][j] /= cluster_count[i];
        }
    }
}


/**
 * Pega os K primeiros pontos como centroids.
 * 
 * @param database
 * @param K
 * 
 * @return
 * */
double** KMeansSeq_CompStartCentroidsDeterministic(int K) {
    double** start_centroids = (double**) malloc(K * sizeof(double*));
    for (next_instance_deterministic = 0; next_instance_deterministic < K; 
            next_instance_deterministic++) {
        start_centroids[next_instance_deterministic] = 
            KMeans_GetCopyFeatures(next_instance_deterministic);
    }

    return start_centroids;
}

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * 
 * @param path_database caminho da base de dados analisada
 * @param K quantidade de clusters

 * */
void KMeans_WriteReport(const char* path_database, int K) {
    int len = strlen(path_database);
    int i;
    char* centroid_file = (char*) malloc((len + CLUSTERS_SUFIX_LENGTH) * sizeof(char));
    FILE* file;

    strcpy(centroid_file, path_database);
    strcpy(&(centroid_file[len]), CLUSTERS_SUFIX);
    file = fopen(centroid_file, "w+");
    if (file == NULL) {
        printf("Erro! Não foi possível abrir o arquivo %s!\n", centroid_file);
        exit(0);
    }
    for (i = 0; i < K; i++) {
        int count = 0;
        fprintf(file, "Cluster %d:\n", i);
        fprintf(file, "Centroid: ");
        Util_WritePoint(file, centroids[i], database->features_length);
        fprintf(file, "\nInstances indexes:\n");
        BOOLEAN print_first = FALSE;
        int sub_cluster_length = KMeans_CountInstancesInCluster(i);
        int* sub_cluster = KMeans_IndexInstancesInCluster(i, sub_cluster_length, 0);
        KMeans_WriteSubCluster(file, &count, sub_cluster_length, sub_cluster, &print_first);
        free(sub_cluster);
        fprintf(file, "\n-------------------------------\n");
    }
    fclose(file);
}

long KMeansSeq_DifTime(struct timeval start, struct timeval end) {
    return (end.tv_sec * 1000000 + end.tv_usec) 
        - (start.tv_sec * 1000000 + start.tv_usec);
}

int KMeansSeq_RunKmeans(int argc, char **argv)
{
    double func_obj_line = 0;
    double func_obj;
    int count_it = 0;
    int K;
    struct timeval start, end;
    struct timeval read_start, read_end;
    struct timeval write_start;

    gettimeofday(&start, NULL);

   // inicializa a semente de aleatoriedade
    srand(time(NULL));

    if (argc != 4) {
        printf("A quantidade de argumentos é inválida!\n");
        exit(0);
    }
    gettimeofday(&read_start, NULL);
    database = DatabaseReader_ReadDatabase(argv[1], atoi(argv[2]));
    gettimeofday(&read_end, NULL);
    K = atoi(argv[3]);

    centroids = KMeansSeq_CompStartCentroidsDeterministic(K);
    func_obj_line = KMeans_CompFuncObj(K);
    // printf("OBJ(%d) -> %lf\n", count_it, func_obj_line);
    // KMeans_PrintCentroids(K);
    do {
        func_obj = func_obj_line;
        KMeansSeq_RecalcClusterCentroids(K);
        func_obj_line = KMeans_CompFuncObj(K);
        count_it++;
        // printf("OBJ(%d) -> %lf\n", count_it, func_obj_line);
        // KMeans_PrintCentroids(K);
    } while (func_obj - func_obj_line > THRESHOLD);
    
    gettimeofday(&write_start, NULL);
    KMeans_WriteReport(argv[1], K);
    gettimeofday(&end, NULL);

    long time_total = KMeansSeq_DifTime(start, end);
    long time_read = KMeansSeq_DifTime(read_start, read_end);
    long time_write = KMeansSeq_DifTime(write_start, end);
    long time_kmeans = time_total - (time_write + time_read);

    printf("%ld,%ld,%ld,%ld", time_read, time_write, time_kmeans, time_total);
    // printf("Tempo total      ->   %ldus\n", time_total);
    // printf("Tempo de leitura ->   %ldus\n", time_read);
    // printf("Tempo de escrita ->   %ldus\n", time_write);
    // printf("Tempo algoritmo  ->   %ldus\n", time_kmeans);

    return 0;
} 