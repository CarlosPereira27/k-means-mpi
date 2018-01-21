#include <mpi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <kmeans_mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    KMeansMPI_RunKmeans(argc, argv);
    MPI_Finalize();
}