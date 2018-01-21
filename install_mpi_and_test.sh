sudo apt-get update

sudo apt-get install mpi-default-dev mpich libopenmpi-dev openmpi-bin

path_databases=("databases/image/segmentation.data")
class_columns=("0")
class_count=("7")
num_features=("19")

echo "Compilando projeto k-means-mpi"
make
echo "----------------------------------------------------"

bin/./bin_database_writer ${path_databases[$0]} ${class_columns[$0]}
mpirun -np 4 bin/./kmeans_mpi_app ${path_databases[$0]}.bin ${num_features[$0]} ${class_count[$0]}