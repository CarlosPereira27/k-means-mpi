#ifndef _KMEANS_MPI_
#define _KMEANS_MPI_

#include <kmeans.h>

#define MASTER_RANK 0
#define TAG 0
#define NO_MORE_REQUEST -1


/**
 *  Número de processos que o k-means está sendo executado.
 * */
int nprocs;
/**
 * Rank do processo que está executando.
 * */
int rank;


/**
 * ---------------------------------------------------------------------------------
 * */


/**
 * Realiza a requisição das features de uma instância ao processo que mantém essa instância.
 * Com o retorno da instância pelo processo que mantém a instância a função retorna a mesma.
 * Essa requisição é síncrona.
 * 
 * @param index índice da instância requisitada
 * 
 * @return features da instância requisitada.
 * */
double* KMeansMPI_RequestInstanceFeatures(unsigned int index);

/**
 * Realiza a requisição de uma instância de forma determínistica.
 * 
 * @return features da instância.
 * */
double* KMeansMPI_DeterministicRequestInstance(void);

/**
 * Realiza a requisição de uma instância de forma aleatória.
 * 
 * @return features da instância.
 * */
double* KMeansMPI_RandomRequestInstance(void);

/**
 * Envia uma instância requisitada para o processo MESTRE.
 * 
 * @param index índice da instância requisitada
 * */
void KMeansMPI_RespondInstance(int index);


/**
 * MESTRE notifica os TRABALHADORES que não haverá mais requisições.
 * */
void KMeansMPI_NotifyNoMoreRequest(void);

/**
 * WORKERS entram em estado de escuta, esperando por requisições de instâncias do MESTRE.
 * E atendem a essas requisições. Até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenInstanceRequest();


/**
 * TRABALHADOR responde ao MESTRE os elementos do cluster solicitado.
 * 
 * @param cluster_id id do cluster que deverá contar as instâncias. Id é também
 *    o índice do cluster
 * */
void KMeansMPI_RespondCluster(int cluster_id);

/**
 * WORKERS entram em estado de escuta, esperando por requisições de clusters do MESTRE.
 * E atendem a essas requisições. Até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenClusterRequest();

/**
 * MESTRE define os centroids iniciais, solicitando as instâncias necessárias
 * para isso. Por fim, notifica os TRABALHADORES que não haverá mais requisições
 * de instâncias.
 * */
void KMeansMPI_DefineStartCentroids(int K);

/**
 * MESTRE realiza a computação dos centroids iniciais, solicitando as instâncias necessárias
 * para isso. Após computar os centroids, atualiza os centroids dos TRABALHADORES por
 * broadcast.  
 * */
void KMeansMPI_CompStartCentroids(int K);

/**
 * Envia os centroids atualizados para todos processos via broadcast.
 * 
 * @param K quantidade de clusteres
 * */
void KMeansMPI_BroadcastUpdatedCentroids(int K);


/**
 * Atualiza todos centroids como a média dos elementos do seu respectivo cluster.
 * 
 * @param K quantidade de clusters
 * @param cluster_count quantidade de elementos em cada cluster
 * @param cluster_sum_elements soma dos elementos de cada cluster
 * */
void KMeansMPI_UpdateCentroids(int K, int* cluster_count, double** cluster_sum_elements);

/**
 * Recalcula os centroids dos clusteres com base na média dos elementos do cluster.
 * 
 * @param K quantidade de clusteres
 * */
void KMeansMPI_RecalcClusterCentroids(int K);

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * 
 * @param path_database caminho da base de dados analisada
 * @param K quantidade de clusters
 * */
void KMeansMPI_WriteReport(const char* path_database, int K);

int KMeansMPI_RunKmeans(int argc, char **argv);

#endif /*_KMEANS_MPI_*/