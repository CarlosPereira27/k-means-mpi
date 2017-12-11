
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include "kmeans.h"
#include "database_handler.h"
#include "array.h"

double** compStartCentroids(Database database, int K) {
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
    printf("\nMinValues:\n");
    printPoint(minValues, database->features_length);
    printf("\nMaxValues:\n");
    printPoint(maxValues, database->features_length);
    printf("\n--------------------------------\n");
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

double difQuadNormVect(double* vect1, double* vect2, int N) {
    int i = 0;
    double difNorm = 0.0;
    for (i = 0; i < N; i++) {
        double dif = vect1[i] - vect2[i];
        difNorm += dif * dif;
    }
    return difNorm;
}



double compJ(Database database, int K, double** clustersCentroid) {
    double bestClusterNorm, normVect;
    int N = database->instances->used;
    int i = 0, k;
    double J = 0.0;
    void** instances = database->instances->data;
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        k = 0;
        bestClusterNorm = difQuadNormVect(instance->features, clustersCentroid[k], 
                database->features_length);
        instance->cluster_id = k;

        for (k = 1; k < K; k++) {
            normVect = difQuadNormVect(instance->features, clustersCentroid[k], 
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


void recalcClusterCentroid(Database database, int K, double** clustersCentroid) {
    int clusterCount[K];
    int i, j;
    for (i = 0; i < K; i++) {
        clusterCount[i] = 0;
        for (j = 0; j < database->features_length; j++) {
            clustersCentroid[i][j] = 0.0;
        }
    }
    int N = database->instances->used;
    void** instances = database->instances->data;
    for (i = 0; i < N; i++) {
        Instance instance = (Instance) instances[i];
        clusterCount[instance->cluster_id]++;
        for (j = 0; j < database->features_length; j++) {
            clustersCentroid[instance->cluster_id][j] += instance->features[j];
        }
    }  
    for (i = 0; i < K; i++) {
        for (j = 0; j < database->features_length; j++) {
            if (clusterCount[i]) {
                clustersCentroid[i][j] /= clusterCount[i];
            }
        }
    }
}

void printListPoints(double** listPoint, int length1, int length2) {
    int i;
    for (i = 0; i < length1; i++) {
        printf("\nPoint %d:\n", i);
        printPoint(listPoint[i], length2);
    }
    printf("\n-----------------------\n");
}

void printPoint(double* point, int length) {
    int i = 0;
    printf("%lf", point[i]);
    for (i++; i < length; i++) {
        printf(", %lf", point[i]);
    }
}

int runKmeans(int argc, char **argv)
{
    if (argc != 4) {
        return 0;
    }
    Database database = readDatabase(argv[1], atoi(argv[2]));
    int K = atoi(argv[3]);

    double** clustersCentroid = compStartCentroids(database, K);
    printf("Primeiros centroids:\n");
    printListPoints(clustersCentroid, K, database->features_length);
    double JLine = compJ(database, K, clustersCentroid);
    printf("J -> %lf\n", JLine);
    double J;
    int countIt = 0;
    do {
        J = JLine;
        recalcClusterCentroid(database, K, clustersCentroid);
        JLine = compJ(database, K, clustersCentroid);
        countIt++;
        printf("J -> %lf\n", J);
    } while (J - JLine > 0.01);
    printf("QTD iteracoes: %d\n", countIt);
    printf("Centroids finais:\n");
    printListPoints(clustersCentroid, K, database->features_length);
    return 0;
} 