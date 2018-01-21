#ifndef _KMEANS_SEQ_
#define _KMEANS_SEQ_

#include <kmeans.h>


/**
 * 
 * */
double** KMeansSeq_CompStartCentroidsX(int K);

/**
 * Recalcula os centroids
 * 
 * */
void KMeansSeq_RecalcClusterCentroid(int K);

/**
 * Pega os K primeiros pontos como centroids.
 * 
 * @param database
 * @param K
 * 
 * @return
 * */
double** KMeansSeq_CompStartCentroidsDeterministic(int K);

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * 
 * @param path_database caminho da base de dados analisada
 * @param K quantidade de clusters

 * */
void KMeans_WriteReport(const char* path_database, int K);

int KMeansSeq_RunKmeans(int argc, char **argv);

#endif /*_KMEANS_SEQ_*/