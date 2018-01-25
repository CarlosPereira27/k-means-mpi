#!/bin/bash

# --------------------------------------------------------
# ---------------- BEGIN Parâmetros ----------------------
# --------------------------------------------------------

# Lista de base de dados originais
path_databases=("databases/iris/iris.data" "databases/image/segmentation.data" "databases/image/segmentation.data10Times" "databases/letter-recognition/letter-recognition.data" "databases/letter-recognition/letter-recognition.data10Times")

# Lista de colunas onde as classes estão localizadas (índices)
class_columns=(4 0 0 0 0)

# Lista da quantidade de classes nas bases de dados
class_count=(3 7 7 26 26)

# Lista da quantidade de features nas bases de dados
num_features=(4 19 19 16 16)

# Lista de quantidade de processos utilizados para cada cenário de teste
num_procs=(2 3 4 6 8 10 12 16 20 24 32 40)

# Arquivo de saída do algoritmo sequencial
out_seq="out_seq.txt"

# Arquivo de saída do algoritmo paralelo
out_par="out_par.txt"

# --------------------------------------------------------
# ---------------- END Parâmetros ------------------------
# --------------------------------------------------------

for i in $(seq 0 $((${#path_databases[@]}-1))); do
    echo "bin/./kmeans_seq_app ${path_databases[$i]} ${class_columns[$i]} ${class_count[$i]} -d > $out_seq"
    bin/./kmeans_seq_app ${path_databases[$i]} ${class_columns[$i]} ${class_count[$i]} -d > $out_seq
    for j in $(seq 0 $((${#num_procs[@]}-1))); do
        echo "mpirun -mca btl ^openib -np ${num_procs[$j]} bin/./kmeans_mpi_app ${path_databases[$i]}.bin ${num_features[$i]} ${class_count[$i]} -d > $out_par"
        mpirun -mca btl ^openib -np ${num_procs[$j]} bin/./kmeans_mpi_app ${path_databases[$i]}.bin ${num_features[$i]} ${class_count[$i]} -d > $out_par
        diff_res=$(diff out_par.txt out_seq.txt)
        if [[ $diff_res != "" ]]; then
            echo ""
            echo "PROBLEMA ENCONTRADO"
            echo "base de dados -> ${path_databases[$i]}"
            echo "número de processos -> ${num_procs[$j]}"
            echo ""
        fi
    done
done