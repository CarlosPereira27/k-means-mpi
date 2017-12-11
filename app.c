#include "mpi.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include "kmeans.h"
#include "kmeans_mpi.h"
#include "database_handler.h"
#include "array.h"

int main(int argc, char **argv)
{
    
    MPI_Init(&argc, &argv);
    int proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc);
    // if (proc == 0) {
    //     runKmeans(argc, argv);
    // }
    runKmeansMPI(argc, argv);
    MPI_Finalize();
    return 0;
} 