#include <mpi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <time.h>
#include <database/database_reader.h>
#include <kmeans.h>

/**
 * Realiza a cópia das features de uma determinada instância da base de dados.
 * 
 * @param index
 *      índice da instância, onde as features serão copiadas
 * 
 * @return cópia das features da instância
 * */
double* KMeans_GetCopyFeatures(int index) {
    double* cp_features = (double*) malloc(database->features_length * sizeof(double));
    Instance instance = database->instances->data[index];

    memcpy(cp_features, instance->features, database->features_length * sizeof(double));

    return cp_features;
}

/**
 * Atualiza o cluster de uma determinada instância. A atualização é feita
 * escolhendo o cluster que contém o centroid em que minimiza o quadrado da 
 * distância em relação a instância. 
 * Retorna o quadrado da distância da instância para o centroid de seu cluster
 * 
 * @param instance
 *      instância a ser alocada em um cluster
 * @param K
 *      quantidade de clusters 
 * 
 * @return quadrado da distância da instância para o centroid de seu cluster
 * */
double KMeans_UpdateClusterElement(Instance instance, int K) {
    double best_centroid_distance;
    double distance_points;
    int k = 0;

    best_centroid_distance = Util_SquareDistance(instance->features, centroids[k], 
            database->features_length);
    instance->cluster_id = k;
    for (k = 1; k < K; k++) {
        distance_points = Util_SquareDistance(instance->features, centroids[k], 
            database->features_length);
        if (distance_points < best_centroid_distance) {
            best_centroid_distance = distance_points;
            instance->cluster_id = k;
        }
    }

    return best_centroid_distance;
}

/**
 * Calcula a função objetivo. A função objetivo é o somatório do quadrado das distâncias 
 * entre as instâncias e os centroids dos clusters em que pertencem. Durante este cálculo, 
 * é atualizado o cluster em que cada instância pertence de forma a minimizar a função 
 * objetivo.
 * 
 * @param K
 *      quantidade de clusters
 * 
 * @return valor da função objetivo.
 * */
double KMeans_CompFuncObj(int K) {
    int N = database->instances->used;
    void** instances = database->instances->data;
    double func_obj = 0.0;
    int i;

    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        func_obj += KMeans_UpdateClusterElement(instance, K);
    }

    return func_obj;
}

/**
 * Define os centróides como o somatórios dos elementos do seu cluster.
 * Também realiza a contagem dos elementos em cada cluster. 
 * 
 * @param K
 *      quantidade de clusters
 * 
 * @return vetor com a contagem dos elementos em cada cluster.
 * */
int* KMeans_SetCentroidsAsSumElements(int K) {
    int* cluster_count = calloc(K, sizeof(int));
    Util_MemsetMatrix((void**) centroids, 0, K, database->features_length * sizeof(double));
    int N = database->instances->used;
    void** instances = database->instances->data;
    int i;
    int j;

    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        cluster_count[instance->cluster_id]++;
        for (j = 0; j < database->features_length; j++) {
            centroids[instance->cluster_id][j] += instance->features[j];
        }
    }
    return cluster_count;
}

/**
 * Atualiza um determinado centroid como a média dos elementos do seu cluster.
 * 
 * @param centroid
 *      centroid a ser atualizado
 * @param sum_elements
 *      somatório dos elementos do cluster
 * @param count_elements
 *      quantidade de elementos no cluster
 * */
void KMeans_UpdateCentroid(double* centroid, double* sum_elements, int count_elements) {
    int j;
    for (j = 0; j < database->features_length; j++) {
        centroid[j] = sum_elements[j] / count_elements;
    }
}

/**
 * Realiza a contagem das instâncias presentes em um determinado cluster.
 * 
 * @param cluster_id
 *      id do cluster que deverá contar as instâncias. Id também é
 *      o índice do cluster
 * 
 * @return quantidade de instâncias contidas no cluster informado
 * */
int KMeans_CountInstancesInCluster(int cluster_id) {
    void** data = database->instances->data;
    int N = database->instances->used;
    int i;
    int count = 0;

    for (i = 0; i < N; i++) {
        Instance instance = (Instance) data[i];
        if (instance->cluster_id == cluster_id) {
            count++;
        }
    }

    return count;
}


/**
 * Escreve os centroids em um determinado arquivo.
 * 
 * @param K
 *      quantidade de centroids
 * */
void KMeans_PrintCentroids(int K) {
    int i;

    for (i = 0; i < K; i++) {
        printf("\nCentroid %d:\n", i);
        Util_PrintPoint(centroids[i], database->features_length);
    }
    printf("\n-----------------------\n");
}


/**
 * Escreve os centroids em um determinado arquivo.
 * 
 * @param file
 *      arquivo em que centroids serão escritos
 * @param centroids
 *      array de centroids
 * @param K
 *      quantidade de centroids
 * */
void KMeans_WriteCentroids(FILE* file, double** centroids, int K) {
    int i;

    for (i = 0; i < K; i++) {
        fprintf(file, "\nCentroid %d:\n", i);
        Util_WritePoint(file, centroids[i], database->features_length);
    }
}

/**
 * Escreve uma instância com todos dados em um arquivo.
 * 
 * @param instance
 *      instância a ser escrita
 * @param features_length
 *      quantidade de features da instância
 * @param file
 *      arquivo em que a instância será escrita
 * */
void KMeans_WriteInstance(Instance instance, int features_length, FILE* file) {
    int i;

    fprintf(file, "%d", instance->class_code);
    for (i = 0; i < features_length; i++) {
        fprintf(file, ",%lf", instance->features[i]);
    }
    fprintf(file, "\n");
}

/**
 * Trata a escrita da primeira instância de uma linha de instâncias de um cluster, 
 * pois essa instância não tem a escrita de uma vírgula a esquerda como o restante. 
 * 
 * @param file
 *      arquivo em que irá escrever o índice da instância
 * @param count_elem
 *      quantidade de elementos do cluster que já foram escritos na linha atual
 * @param sub_cluster_length
 *      tamanho do subcluster a ser escrito
 * @param sub_cluster
 *      subcluster a ser escrito
 * @param print_first
 *      se TRUE indica que o primeiro elemento do subcluster é o primeiro 
 *      elemento da linha a ser escrita, caso contrário é FALSE
 * */
int* KMeans_WriteFirstInstanceLine(FILE* file, int* count_elem, int* sub_cluster_length, 
            int* sub_cluster, BOOLEAN* print_first) {
    if (!(*print_first) && (*sub_cluster_length) > 0) {
        fprintf(file, "%d", sub_cluster[0]);
        (*print_first) = TRUE;
        (*sub_cluster_length) = (*sub_cluster_length) - 1;
        sub_cluster = &(sub_cluster[1]);
        (*count_elem) = (*count_elem) + 1;
    }
    return sub_cluster;
}

/**
 * Escreve os índices de um determinado subcluster em um determinado arquivo. 
 * 
 * @param file
 *      arquivo em que irá escrever os índices dos elementos do subcluster
 * @param count_elem
 *      quantidade de elementos do cluster que já foram escritos na linha atual
 * @param sub_cluster_length
 *      tamanho do subcluster a ser escrito
 * @param sub_cluster
 *      subcluster a ser escrito
 * @param print_first
 *      se TRUE indica que o primeiro elemento do subcluster é o primeiro 
 *      elemento da linha a ser escrita, caso contrário é FALSE
 * */
void KMeans_WriteSubCluster(FILE* file, int* count_elem, int sub_cluster_length, 
            int* sub_cluster, BOOLEAN* print_first) {
    int i;
    int* sub_cluster_real = sub_cluster;
    while (*count_elem + sub_cluster_length >= INDEX_PER_LINE) {
        sub_cluster = KMeans_WriteFirstInstanceLine(file, count_elem, &sub_cluster_length, 
            sub_cluster, print_first);
        int max = INDEX_PER_LINE - *count_elem;
        for (i = 0; i < max; i++) {
            fprintf(file, ",%d", sub_cluster[i]);
        }
        fprintf(file, "\n");
        (*print_first) = FALSE;
        *count_elem = 0;
        sub_cluster_length -= (max);
        sub_cluster = &(sub_cluster[max]);
    }
    sub_cluster = KMeans_WriteFirstInstanceLine(file, count_elem, &sub_cluster_length, 
        sub_cluster, print_first);
    *count_elem += sub_cluster_length;
    for (i = 0; i < sub_cluster_length; i++) {
        fprintf(file, ",%d", sub_cluster[i]);
    }
    sub_cluster = sub_cluster_real;
}

/**
 * Recupera um vetor com os índices das instâncias de um determinado cluster.
 * 
 * @param cluster_id
 *      id do cluster que deverá contar as instâncias. Id também é
 *      o índice do cluster
 * @param length
 *      quantidade de elementos no cluster
 * 
 * @return vetor com os índices das instâncias do cluster
 * */
int* KMeans_IndexInstancesInCluster(int cluster_id, int length, int offset) {
    int* cluster = (int*) malloc(length * sizeof(int));
    int N = database->instances->used;
    int i;
    void** data = database->instances->data;
    int count = 0;

    for (i = 0; i < N; i++) {
        Instance instance = (Instance) data[i];
        if (instance->cluster_id == cluster_id) {
            cluster[count] = i + offset;
            count++;
        }
    }

    return cluster;
}