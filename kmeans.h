#ifndef _KMEANS_
#define _KMEANS_

#include "database_handler.h"
#include "array.h"

#define THRESHOLD 0.01

double** compStartCentroids(Database database, int K);
double difQuadNormVect(double* vect1, double* vect2, int N);
double compJ(Database database, int K, double** clustersCentroid);
void recalcClusterCentroid(Database database, int K, double** clustersCentroid);
void printListPoints(double** listPoint, int length1, int length2);
int runKmeans(int argc, char **argv);
void printPoint(double* point, int length);

#endif /*_KMEANS_*/