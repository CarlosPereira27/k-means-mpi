#ifndef _KMEANS_SEQ_
#define _KMEANS_SEQ_

#include <kmeans.h>

/**
 * Define os centroids iniciais com pontos aleatórios entre os limites de cada 
 * feature da base de dados.
 * 
 * @param K
 *      quantidade de clusters
 * */
void KMeansSeq_DefineStartCentroidsX(int K);

/**
 * Recalcula os centroids dos clusteres com base na média dos elementos 
 * de cada cluster.
 * 
 * @param K quantidade de clusteres
 * */
void KMeansSeq_RecalcClusterCentroid(int K);

/**
 * Define os centroids iniciais, gerando K índices de instâncias para
 * definir como centroids iniciais.
 * 
 * @param K
 *      quantidade de clusters
 * 
 * @return centroids iniciais
 * */
void KMeansSeq_DefineStartCentroids(int K);

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * O relatório informa os K clusters criados com seus respectivos centroids e coleção 
 * de índices das instâncias que pertencem a este cluster. 
 * 
 * @param path_database
 *      caminho da base de dados analisada
 * @param K
 *      quantidade de clusters
 * */
void KMeans_WriteReport(const char* path_database, int K);

/**
 * Calcula a diferença entre tempos na estrutura timeval retornando o
 * tempo em microssegundos.
 * 
 * @param start
 *      tempo inicial
 * @param end
 *      tempo final
 * 
 * @return diferença entre tempo inicial e final em microssegundos
 * */
long KMeansSeq_DifTime(struct timeval start, struct timeval end);

/**
 * Executa o KMeans de forma sequencial.
 * */
int KMeansSeq_RunKmeans(int argc, char **argv);

#endif /*_KMEANS_SEQ_*/