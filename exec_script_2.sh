#!/bin/bash

# --------------------------------------------------------
# ---------------- BEGIN Parâmetros ----------------------
# --------------------------------------------------------

# Lista de base de dados originais
path_databases=("databases/image/segmentation.data" "databases/image/segmentation.data10Times" "databases/letter-recognition/letter-recognition.data" "databases/letter-recognition/letter-recognition.data10Times")

# Lista de colunas onde as classes estão localizadas (índices)
class_columns=(0 0 0 0)

# Lista da quantidade de classes nas bases de dados
class_count=(7 7 26 26)

# Lista da quantidade de features nas bases de dados
num_features=(19 19 16 16)

# Quantidade de vezes que executará cada cenário de teste
qty_test=10

# Lista de quantidade de processos utilizados para cada cenário de teste
#um_procs=(2 3 4 6 8 10 12 16 20 24 32 40)

# Lista de arquivos de saída csv para os tempos de processamento
out_files=("reports/segmentation/segmentation.csv" "reports/segmentation_10times/segmentation_10times.csv" "reports/letter-recognition/letter-recognition.csv" "reports/letter-recognition_10times/letter-recognition_10times.csv")

# --------------------------------------------------------
# ---------------- END Parâmetros ------------------------
# --------------------------------------------------------



echo "Compilando projeto k-means-mpi"
make
echo "----------------------------------------------------"
echo

echo "Criando diretórios de relatório ..."
mkdir -p reports/segmentation
mkdir -p reports/segmentation_10times
mkdir -p reports/letter-recognition
mkdir -p reports/letter-recognition_10times
echo

echo "Criando headers para arquivos de relatórios ..."
for i in $(seq 0 $((${#out_files[@]}-1))); do
    echo "Criando header do arquivo de saída csv - ${out_files[$i]}"
    echo "time_in,time_report,time_kmeans,time_total,nproc,teste_index" > ${out_files[$i]}
done
echo "----------------------------------------------------"


echo "Gerando versão binária das base de dados com a aplicação bin_database_writer"
for i in $(seq 0 $((${#path_databases[@]}-1))); do
    echo "bin/./bin_database_writer ${path_databases[$i]} ${class_columns[$i]}Z"
    bin/./bin_database_writer ${path_databases[$i]} ${class_columns[$i]}
    echo "----------------------------------------------------"
done
echo "----------------------------------------------------"

for k in $(seq 0 $((qty_test-1))); do
    for i in $(seq 0 $((${#path_databases[@]}-1))); do
        for j in $(seq 0 $((${#num_procs[@]}-1))); do
                echo "mpirun -mca btl ^openib -np ${num_procs[$j]} bin/./kmeans_mpi_app ${path_databases[$i]}.bin ${num_features[$i]} ${class_count[$i]} >> ${out_files[$i]}"
                mpirun -mca btl ^openib -np ${num_procs[$j]} bin/./kmeans_mpi_app ${path_databases[$i]}.bin ${num_features[$i]} ${class_count[$i]} >> ${out_files[$i]}
                echo ",${num_procs[$j]},$k" >> ${out_files[$i]}
        done
        echo "bin/./kmeans_seq_app ${path_databases[$i]} ${class_columns[$i]} ${class_count[$i]} >> ${out_files[$i]}"
        bin/./kmeans_seq_app ${path_databases[$i]} ${class_columns[$i]} ${class_count[$i]} >> ${out_files[$i]}
        echo ",1,$k" >> ${out_files[$i]}
    done
done
