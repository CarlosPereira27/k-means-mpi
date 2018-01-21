#ifndef _KMEANS_
#define _KMEANS_

#include <util.h>
#include <database/database_struct.h>

#define THRESHOLD 0.01
#define CENTROID_SUFIX ".centroids"
#define CENTROID_SUFIX_LENGTH 11
#define CLUSTERS_SUFIX ".clusters"
#define CLUSTERS_SUFIX_LENGTH 10
#define INDEX_PER_LINE 50

/**
 * Base de dados do processo que está executando.
 * */
Database database;

/**
 * Centroids dos atuais clusters.
 * */
double** centroids;

/**
 * Quantidade de instâncias na base de dados.
 * */
int num_instances_g;

/* ---------------------------------------------------------------------------- */

/**
 * Realiza a cópia das features de uma determinada instância da base de dados.
 * 
 * @param index índice da instância, onde as features serão copiadas
 * 
 * @return cópia das features da instância
 * */
double* KMeans_GetCopyFeatures(int index);

/**
 * Atualiza o cluster de uma determinada instância. A atualização é feita
 * escolhendo o cluster que contém o centroid em que minimizao quadrado da 
 * distância em relação a instância. 
 * Retorna o quadrado da distância da instância para o centroid de seu cluster
 * 
 * @param instance instância a ser alocada em um cluster
 * @param K quantidade de clusters 
 * 
 * @return quadrado da distância da instância para o centroid de seu cluster
 * */
double KMeans_UpdateClusterElement(Instance instance, int K);

/**
 * Calcula a função objetivo. A função objetivo é o somatório do quadrado das distâncias 
 * entre as instâncias e os centroids dos clusters em que estão alocadas. Durante este
 * cálculo, é atualizado o cluster em que cada instância deve ser alocada de forma a 
 * minimizar a função objetivo.
 * 
 * @param K quantidade de cluster
 * 
 * @return valor da função objetivo.
 * */
double KMeans_CompFuncObj(int K);

/**
 * Define os centróides como o somatórios dos elementos do seu cluster.
 * Também realiza a contagem dos elementos em cada cluster. 
 * 
 * @param K quantidade de clusters
 * 
 * @return vetor com a contagem dos elementos em cada cluster.
 * */
int* KMeans_SetCentroidsAsSumElements(int K);

/**
 * Atualiza um determinado centroid como a média dos elementos do seu cluster.
 * 
 * @param centroid centroid a ser atualizado
 * @param sum_elements somatório dos elementos do cluster
 * @param count_elements quantidade de elementos no cluster
 * */
void KMeans_UpdateCentroid(double* centroid, double* sum_elements, int count_elements);

/**
 * Realiza a contagem das instâncias presentes em um determinado cluster.
 * 
 * @param cluster_id id do cluster que deverá contar as instâncias. Id é também
 *    o índice do cluster
 * 
 * @return quantidade de instâncias contidas no cluster informado
 * */
int KMeans_CountInstancesInCluster(int cluster_id);

/**
 * Escreve os centroids em um determinado arquivo.
 * 
 * @param K quantidade de centroids
 * */
void KMeans_PrintCentroids(int K);

/**
 * Escreve os centroids em um determinado arquivo.
 * 
 * @param file arquivo em que centroids serão escritos
 * @param centroids_l array de centroids
 * @param K quantidade de centroids
 * */
void KMeans_WriteCentroids(FILE* file, double** centroids, int K);

/**
 * Escreve uma instância com todos dados em um arquivo.
 * 
 * @param instance instância a ser escrita
 * @param features_length quantidade de features da instância
 * @param file arquivo em que a instância será escrita
 * */
void KMeans_WriteInstance(Instance instance, int features_length, FILE* file);

/**
 * Trata a escrita da primeira instância de uma linha de instâncias de um cluster, 
 * pois essa instância não tem a escrita de uma vírgula a esquerda como o restante. 
 * 
 * @param file arquivo em que irá escrever o índice da instância
 * @param count_elem quantidade de elementos do cluster que já foram escritos na
 *          linha atual
 * @param sub_cluster_length tamanho do subcluster a ser escrito
 * @param sub_cluster subcluster a ser escrito
 * @param print_first se TRUE indica que o primeiro elemento do subcluster é o primeiro 
 *          elemento da linha a ser escrita, caso contrário é FALSE
 * */
int* KMeans_WriteFirstInstanceLine(FILE* file, int* count_elem, int* sub_cluster_length, 
            int* sub_cluster, BOOLEAN* print_first);

/**
 * Escreve os índices de um determinado subcluster em um determinado arquivo. 
 * 
 * @param file arquivo em que irá escrever os índices dos elementos do subcluster
 * @param count_elem quantidade de elementos do cluster que já foram escritos na
 *          linha atual
 * @param sub_cluster_length tamanho do subcluster a ser escrito
 * @param sub_cluster subcluster a ser escrito
 * @param print_first se TRUE indica que o primeiro elemento do subcluster é o primeiro 
 *          elemento da linha a ser escrita, caso contrário é FALSE
 * */
void KMeans_WriteSubCluster(FILE* file, int* count_elem, int sub_cluster_length, 
            int* sub_cluster, BOOLEAN* print_first);

/**
 * Recupera um vetor com os índices das instâncias de um determinado cluster.
 * 
 * @param cluster_id id do cluster que deverá contar as instâncias. Id é também
 *    o índice do cluster
 * @param length quantidade de elementos no cluster
 * 
 * @return vetor com os índices das instâncias do cluster
 * */
int* KMeans_IndexInstancesInCluster(int cluster_id, int length, int offset);

#endif /*_KMEANS_*/