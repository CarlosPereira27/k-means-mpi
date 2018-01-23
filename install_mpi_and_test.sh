#!/bin/bash

# --------------------------------------------------------
# ---------------- BEGIN Parâmetros ----------------------
# --------------------------------------------------------

# Base de dados de teste
path_database="databases/image/segmentation.data"

# Índice da coluna que contém a classe na base de dados
class_column=0

# Quantidade de classes diferentes da base de dados
class_count=7

# Quantidade de features das instâncias da base de dados
num_features=19

# --------------------------------------------------------
# ---------------- END Parâmetros ------------------------
# --------------------------------------------------------

echo "Instalando mpi"

sudo apt-get update
sudo apt-get install mpi-default-dev mpich libopenmpi-dev openmpi-bin

echo "Compilando projeto k-means-mpi"
make
echo "----------------------------------------------------"

echo "Testando ..."
echo "bin/./bin_database_writer $path_database $class_column"
bin/./bin_database_writer $path_database $class_column
echo "mpirun -mca btl ^openib -np 4 bin/./kmeans_mpi_app $path_database.bin $num_features $class_count"
mpirun -mca btl ^openib -np 4 bin/./kmeans_mpi_app $path_database.bin $num_features $class_count
echo ""

