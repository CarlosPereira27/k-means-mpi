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
 * MESTRE
 *      Se possui a instância requisitada, retorna uma cópia de suas features.
 *      Senão, requisita instância ao processo TRABALHADOR que mantém essa instância. 
 *      Com o retorno da instância pelo processo que mantém a instância a função
 *      retorna a mesma. Essa requisição é síncrona.
 * 
 * @param index
 *      índice da instância requisitada
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
 * TRABALHADOR
 *      Envia uma instância requisitada para o processo MESTRE.
 * 
 * @param index
 *      índice da instância requisitada
 * */
void KMeansMPI_RespondInstance(int index);

/**
 * MESTRE
 *      notifica os TRABALHADORES que não haverá mais requisições.
 * */
void KMeansMPI_NotifyNoMoreRequest(void);

/**
 * TRABALHADORES
 *      Entram em estado de escuta, esperando por requisições de instâncias do MESTRE.
 *      Atendem a essas requisições até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenInstanceRequest();

/**
 * MESTRE envia os centroids atualizados para todos processos 
 * TRABALHADORES via broadcast.
 * 
 * @param K
 *      quantidade de clusteres
 * */
void KMeansMPI_BroadcastUpdatedCentroids(int K);

/**
 * MESTRE
 *      Define os centroids iniciais, gerando K índices de instâncias para
 *      definir como centroids iniciais. Solicita as instâncias que não 
 *      possui para o processo TRABALHADOR que a possui. Por fim, notifica os 
 *      TRABALHADORES que não haverá mais requisições de instâncias.
 * TRABALHADORES
 *      Escutam e respondem as requisições de instâncias do MESTRE.
 * 
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_DefineStartCentroids(int K);

/**
 * MESTRE
 *      Define os centroids iniciais
 * TRABALHADORES
 *      Escutam e respondem as requisições de instâncias do MESTRE
 * MESTRE envia centroids iniciais via broadcast a todos 
 * processos TRABALHADORES 
 * 
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_CompStartCentroids(int K);

/**
 * Atualiza todos centroids como a média dos elementos do seu respectivo cluster.
 * Caso algum cluster esteja vazio, usa uma instância como centroid para esse cluster.
 * 
 * MESTRE
 *      Para cada cluster nos clusters
 *          Se cluster possui elementos então
 *              calcula o centroid do cluster
 *          senão
 *              usa uma instância como centroid para esse cluster
 *              se não possui instância então solicita instância ao processo
 *              que mantém essa instância
 * TRABALHADORES
 *      Escutam e repondem requisições de instância realizadas pelo MESTRE até
 *      o mestre informar que não haverá mais requisições
 *      
 * @param K
 *      quantidade de clusters
 * @param cluster_count
 *      quantidade de elementos em cada cluster
 * @param cluster_sum_elements
 *      soma dos elementos de cada cluster
 * */
void KMeansMPI_UpdateCentroids(int K, int* cluster_count, double** cluster_sum_elements);

/**
 * Recalcula os centroids dos clusteres com base na média dos elementos 
 * de cada cluster.
 * 
 * Calcula a lista de pontos que representam o somatório das instâncias
 * pertencentes a cada cluster.
 * Calcula a lista de tamanhos de cada cluster.
 * Reduz o somatório das listas de pontos que representam o somatório
 * das instâncias pertencentes a cada cluster no processo MESTRE.
 * Reduz o somatório das listas de tamanhos de cada cluster no processo
 * MESTRE.
 * MESTRE
 *      Atualiza os pontos centroids.
 * TRABALHADORES
 *      Escutam e repondem requisições de instância realizadas pelo MESTRE até
 *      o mestre informar que não haverá mais requisições.
 * MESTRE envia novos centroids via broadcast a todos processos TRABALHADORES.
 * 
 * @param K
 *      quantidade de clusteres
 * */
void KMeansMPI_RecalcClusterCentroids(int K);

/**
 * TRABALHADOR
 *      responde ao MESTRE o tamanho e os índices das instâncias pertencentes
 *      ao cluster solicitado.
 * 
 * @param cluster_id
 *      id do cluster solicitado
 * */
void KMeansMPI_RespondCluster(int cluster_id);

/**
 * TRABALHADORES
 *      Entram em estado de escuta, esperando por requisições de clusters do MESTRE.
 *      Atendem a essas requisições até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenClusterRequest();

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * O relatório informa os K clusters criados com seus respectivos centroids e coleção 
 * de índices das instâncias que pertencem a este cluster. 
 * MESTRE
 *      Escreve relatório, e requisita tamanho e índices de instâncias de clusters aos 
 *      processos TRABALHADORES.
 *      Avisa trabalhadores que não haverá mais requisições.
 * TRABALHADORES
 *      Escutam e respondem as requisições de tamanho e índices de instâncias de clusters
 *      ao processo MESTRE até o mesmo informar que não haverá mais requisições.
 * 
 * 
 * @param path_database
 *      caminho da base de dados analisada
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_WriteReport(const char* path_database, int K);

/**
 * Executa o KMeans de forma paralela com multiprocessos.
 * */
int KMeansMPI_RunKmeans(int argc, char **argv);

#endif /*_KMEANS_MPI_*/