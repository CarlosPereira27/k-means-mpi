#ifndef _KMEANS_MPI_
#define _KMEANS_MPI_

#include "database_handler.h"

#define THRESHOLD_MPI 0.01
#define MASTER_RANK 0
#define BOOLEAN int
#define TRUE 1
#define FALSE 0
#define TAG 0

void writeInstanceMPI(Instance instance, int features_length, MPI_File file);
void writeInstance(Instance instance, int features_length, FILE* file);
double* requestCentroid(int unsigned index);
void respondCentroid(int index);
double* getCopyFeatures(int index);
BOOLEAN containsValue(int* array, int length, int value);
int* randomKIndexes(int K);
void centroidsComputed(void);
void compStartCentroidsMPI(int K);
double difQuadNormVectMPI(double* vect1, double* vect2, int N);
double compJMPI(int K);
void recalcClusterCentroidMPI(int K);
void printListPointsMPI(double** listPoint, int length1, int length2);
void printPointMPI(double* point, int length);
int runKmeansMPI(int argc, char **argv);


#endif /*_KMEANS_MPI_*/