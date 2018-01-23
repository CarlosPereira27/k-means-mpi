#include <mpi.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <limits.h>
#include <time.h>
#include <database/bin_database_reader.h>
#include <kmeans_mpi.h>

/**
 * MESTRE
 *      Se possui a instância requisitada, retorna uma cópia de suas features.
 *      Senão, requisita instância ao processo TRABALHADOR que mantém essa instância. 
 *      Com o retorno da instância pelo processo que mantém a instância a função
 *      retorna a mesma. Essa requisição é síncrona.
 * 
 * @param index
 *      índice da instância requisitada
 * 
 * @return features da instância requisitada.
 * */
double* KMeansMPI_RequestInstanceFeatures(unsigned int index) {
    if (index < database->instances->used) {
        return KMeans_GetCopyFeatures(index);
    }

    double* instance = (double*) malloc(database->features_length * sizeof(double));
    int rank_request = index / database->instances->used;

    index %= database->instances->used;
    if (rank_request == nprocs) {
        rank_request--;
        index += database->instances->used;
    }
    // SOLICITANDO instância de índice index para o processo de rank rank_request.
    MPI_Send(&index, 1, MPI_INT, rank_request, TAG, MPI_COMM_WORLD);
    // RECEBENDO instância de índice index do processo de rank rank_request.
    MPI_Recv(instance, database->features_length, MPI_DOUBLE, rank_request, 
        TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return instance;
}

/**
 * Realiza a requisição de uma instância de forma determínistica.
 * 
 * @return features da instância.
 * */
double* KMeansMPI_DeterministicRequestInstance(void) {
    if (next_instance_deterministic == num_instances_g) {
        next_instance_deterministic = 0;
    }
    return KMeansMPI_RequestInstanceFeatures(next_instance_deterministic++);
}

/**
 * Realiza a requisição de uma instância de forma aleatória.
 * 
 * @return features da instância.
 * */
double* KMeansMPI_RandomRequestInstance(void) {
    return KMeansMPI_RequestInstanceFeatures(Util_GenerateRandomIntValue(num_instances_g));
}

/**
 * TRABALHADOR
 *      Envia uma instância requisitada para o processo MESTRE.
 * 
 * @param index
 *      índice da instância requisitada
 * */
void KMeansMPI_RespondInstance(int index) {
    Instance instance = (Instance) database->instances->data[index];

    // ENVIANDO instância de índice index para o MESTRE.
    MPI_Send(instance->features, database->features_length, MPI_DOUBLE, 
        MASTER_RANK, TAG, MPI_COMM_WORLD);
}

/**
 * MESTRE
 *      notifica os TRABALHADORES que não haverá mais requisições.
 * */
void KMeansMPI_NotifyNoMoreRequest(void) {
    int i;
    int no_more_request = NO_MORE_REQUEST;

    for (i = 0; i < nprocs; i++) {
        if (i != MASTER_RANK) {
            // INFORMANDO o processo de rank i que não haverá mais requisições 
            // realizadas pelo processo MESTRE.
            MPI_Send(&no_more_request, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }
    }
}

/**
 * TRABALHADORES
 *      Entram em estado de escuta, esperando por requisições de instâncias do MESTRE.
 *      Atendem a essas requisições até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenInstanceRequest() {
    int index_request;

    // RECEBENDO requisição de instância de índice index_request do processo mestre.
    MPI_Recv(&index_request, 1, MPI_INT, MASTER_RANK, 
        TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Enquanto houver requisições do processo mestre, elas serão atendidas.
    while (index_request != NO_MORE_REQUEST) {
        // ENVIANDO instância de índice index_request para o processo mestre.
        KMeansMPI_RespondInstance(index_request);
        // RECEBENDO requisição de instância de índice index_request do processo mestre.
        MPI_Recv(&index_request, 1, MPI_INT, MASTER_RANK, 
            TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

/**
 * MESTRE envia os centroids atualizados para todos processos 
 * TRABALHADORES via broadcast.
 * 
 * @param K
 *      quantidade de clusteres
 * */
void KMeansMPI_BroadcastUpdatedCentroids(int K) {
    int i;
    for (i = 0; i < K; i++) {
        // ATUALIZA os centroids em todos processos por broadcast.
        MPI_Bcast(centroids[i], database->features_length, MPI_DOUBLE, 
            MASTER_RANK, MPI_COMM_WORLD);
    }
}

/**
 * MESTRE
 *      Define os centroids iniciais, gerando K índices de instâncias para
 *      definir como centroids iniciais. Solicita as instâncias que não 
 *      possui para o processo TRABALHADOR que a possui. Por fim, notifica os 
 *      TRABALHADORES que não haverá mais requisições de instâncias.
 * TRABALHADORES
 *      Escutam e respondem as requisições de instâncias do MESTRE.
 * 
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_DefineStartCentroids(int K) {
    int* start_centroids_index = KMeans_GenKIndexes(K, num_instances_g);
    int i = 0;
    for (i = 0; i < K; i++) {
        centroids[i] = 
            KMeansMPI_RequestInstanceFeatures(start_centroids_index[i]);
    }
    free(start_centroids_index);
    KMeansMPI_NotifyNoMoreRequest();
    next_instance_deterministic = K;
}

/**
 * MESTRE
 *      Define os centroids iniciais
 * TRABALHADORES
 *      Escutam e respondem as requisições de instâncias do MESTRE
 * MESTRE envia centroids iniciais via broadcast a todos 
 * processos TRABALHADORES 
 * 
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_CompStartCentroids(int K) {
    int i;

    centroids = (double**) malloc(K * sizeof(double*));
    if (rank == MASTER_RANK) {
        KMeansMPI_DefineStartCentroids(K);
    } else {
        for (i = 0; i < K; i++) {
            centroids[i] = (double*) malloc(database->features_length * sizeof(double));
        }
        KMeansMPI_ListenInstanceRequest();
    }
    KMeansMPI_BroadcastUpdatedCentroids(K);
}

/**
 * Atualiza todos centroids como a média dos elementos do seu respectivo cluster.
 * Caso algum cluster esteja vazio, usa uma instância como centroid para esse cluster.
 * 
 * MESTRE
 *      Para cada cluster nos clusters
 *          Se cluster possui elementos então
 *              calcula o centroid do cluster
 *          senão
 *              usa uma instância como centroid para esse cluster
 *              se não possui instância então solicita instância ao processo
 *              que mantém essa instância
 * TRABALHADORES
 *      Escutam e repondem requisições de instância realizadas pelo MESTRE até
 *      o mestre informar que não haverá mais requisições
 *      
 * @param K
 *      quantidade de clusters
 * @param cluster_count
 *      quantidade de elementos em cada cluster
 * @param cluster_sum_elements
 *      soma dos elementos de cada cluster
 * */
void KMeansMPI_UpdateCentroids(int K, int* cluster_count, double** cluster_sum_elements) {
    int i;
    for (i = 0; i < K; i++) {
        if (cluster_count[i]) {
            KMeans_UpdateCentroid(centroids[i], cluster_sum_elements[i], cluster_count[i]);
        } else {
            centroids[i] = KMeans_RequestNextInstanceFeatures();
        }
        free(cluster_sum_elements[i]);
    }
    free(cluster_count);
    KMeansMPI_NotifyNoMoreRequest();
}

/**
 * Recalcula os centroids dos clusteres com base na média dos elementos 
 * de cada cluster.
 * 
 * Calcula a lista de pontos que representam o somatório das instâncias
 * pertencentes a cada cluster.
 * Calcula a lista de tamanhos de cada cluster.
 * Reduz o somatório das listas de pontos que representam o somatório
 * das instâncias pertencentes a cada cluster no processo MESTRE.
 * Reduz o somatório das listas de tamanhos de cada cluster no processo
 * MESTRE.
 * MESTRE
 *      Atualiza os pontos centroids.
 * TRABALHADORES
 *      Escutam e repondem requisições de instância realizadas pelo MESTRE até
 *      o mestre informar que não haverá mais requisições.
 * MESTRE envia novos centroids via broadcast a todos processos TRABALHADORES.
 * 
 * @param K
 *      quantidade de clusteres
 * */
void KMeansMPI_RecalcClusterCentroids(int K) {
    int* cluster_count = KMeans_SetCentroidsAsSumElements(K);
    int i;

    int* sum_cluster_count = NULL;
    double** sum_cluster_elements = (double**) malloc(K * sizeof(double*));

    if (rank == MASTER_RANK) {
        sum_cluster_count = (int*) calloc(K, sizeof(int));
        Util_CallocMatrix((void**) sum_cluster_elements, K, database->features_length, sizeof(double));
    }

    // Reduz o somatório da quantidade de elementos em cada cluster.
    MPI_Reduce(cluster_count, sum_cluster_count, K, MPI_INT, MPI_SUM, 
        MASTER_RANK, MPI_COMM_WORLD);
    free(cluster_count);
    for (i = 0; i < K; i++) {
        // Reduz o somatório dos elementos pertencentes ao cluster i. 
        MPI_Reduce(centroids[i], sum_cluster_elements[i], database->features_length, 
            MPI_DOUBLE, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
    }

    if (rank == MASTER_RANK) {
        KMeansMPI_UpdateCentroids(K, sum_cluster_count, sum_cluster_elements);
    } else {
        KMeansMPI_ListenInstanceRequest();
    }
    free(sum_cluster_elements);

    KMeansMPI_BroadcastUpdatedCentroids(K);
}

/**
 * TRABALHADOR
 *      responde ao MESTRE o tamanho e os índices das instâncias pertencentes
 *      ao cluster solicitado.
 * 
 * @param cluster_id
 *      id do cluster solicitado
 * */
void KMeansMPI_RespondCluster(int cluster_id) {
    int sub_cluster_length = KMeans_CountInstancesInCluster(cluster_id);
    int* sub_cluster = KMeans_IndexInstancesInCluster(cluster_id, sub_cluster_length, 
            (num_instances_g / nprocs) * rank);
    // ENVIANDO tamanho do cluster para o MESTRE
    MPI_Send(&sub_cluster_length, 1, MPI_INT, 
        MASTER_RANK, TAG, MPI_COMM_WORLD);
    // ENVIANDO cluster para o MESTRE
    MPI_Send(sub_cluster, sub_cluster_length, MPI_INT, 
        MASTER_RANK, TAG, MPI_COMM_WORLD);
}

/**
 * TRABALHADORES
 *      Entram em estado de escuta, esperando por requisições de clusters do MESTRE.
 *      Atendem a essas requisições até o MESTRE informar que não haverá mais requisições.
 * */
void KMeansMPI_ListenClusterRequest() {
    int index_request;

    // RECEBENDO requisição de cluster de índice index_request do processo mestre.
    MPI_Recv(&index_request, 1, MPI_INT, MASTER_RANK, 
        TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Enquanto houver requisições do processo mestre, elas serão atendidas.
    while (index_request != NO_MORE_REQUEST) {
        // ENVIANDO cluster de índice index_request para o processo mestre.
        KMeansMPI_RespondCluster(index_request);
        // RECEBENDO requisição de cluster de índice index_request do processo mestre.
        MPI_Recv(&index_request, 1, MPI_INT, MASTER_RANK, 
            TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

/**
 * Escreve o relatório com o resultado do KMeans em uma arquivo que possui o seguinte
 * caminho -> '$caminho_banco_de_dados' + CLUSTERS_SUFIX.
 * O relatório informa os K clusters criados com seus respectivos centroids e coleção 
 * de índices das instâncias que pertencem a este cluster. 
 * MESTRE
 *      Escreve relatório, e requisita tamanho e índices de instâncias de clusters aos 
 *      processos TRABALHADORES.
 *      Avisa trabalhadores que não haverá mais requisições.
 * TRABALHADORES
 *      Escutam e respondem as requisições de tamanho e índices de instâncias de clusters
 *      ao processo MESTRE até o mesmo informar que não haverá mais requisições.
 * 
 * 
 * @param path_database
 *      caminho da base de dados analisada
 * @param K
 *      quantidade de clusters
 * */
void KMeansMPI_WriteReport(const char* path_database, int K) {
    int len = strlen(path_database);
    int i;
    int j;
    char* centroid_file = (char*) malloc((len + CLUSTERS_SUFIX_LENGTH) * sizeof(char));
    FILE* file;

    strcpy(centroid_file, path_database);
    strcpy(&(centroid_file[len]), CLUSTERS_SUFIX);
    file = fopen(centroid_file, "w+");
    if (file == NULL) {
        printf("Erro! Rank (%d) não foi possível abrir o arquivo %s!\n", rank, centroid_file);
        exit(0);
    }
    for (i = 0; i < K; i++) {
        int count = 0;
        fprintf(file, "Cluster %d:\n", i);
        fprintf(file, "Centroid: ");
        Util_WritePoint(file, centroids[i], database->features_length);
        fprintf(file, "\nInstances indexes:\n");
        BOOLEAN print_first = FALSE;
        int sub_cluster_length = KMeans_CountInstancesInCluster(i);
        int* sub_cluster = KMeans_IndexInstancesInCluster(i, sub_cluster_length, 
                (num_instances_g / nprocs) * rank);
        KMeans_WriteSubCluster(file, &count, sub_cluster_length, sub_cluster, &print_first);
        for (j = 0; j < nprocs; j++) {
            if (j == MASTER_RANK) {
                continue;
            }
            // SOLICITA tamanho do cluster i e array com índíces das instâncias do 
            // cluster i ao processo j
            MPI_Send(&i, 1, MPI_INT, j, TAG, MPI_COMM_WORLD);
            MPI_Recv(&sub_cluster_length, 1, MPI_INT, j, 
                TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            free(sub_cluster);
            sub_cluster = (int*) malloc(sub_cluster_length * sizeof(int));
            MPI_Recv(sub_cluster, sub_cluster_length, MPI_INT, j, 
                TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            KMeans_WriteSubCluster(file, &count, sub_cluster_length, sub_cluster, &print_first);
        }
        free(sub_cluster);
        fprintf(file, "\n-------------------------------\n");
    }
    fclose(file);
    KMeansMPI_NotifyNoMoreRequest();
}


/**
 * Executa o KMeans de forma paralela com multiprocessos.
 * */
int KMeansMPI_RunKmeans(int argc, char **argv) {
    double func_obj_line_local;
    double func_obj_line_global = 0;
    double func_obj;
    int count_it = 0;
    int K;
    double start_time, end_time;
    double read_start_time, read_end_time;
    double write_start_time;


    // Inicializa a semente de aleatoriedade
    srand(time(NULL));
    next_instance_deterministic = 0;
    KMeans_GenKIndexes = Util_GenerateDeterministicKIntValues;
    KMeans_RequestNextInstanceFeatures = KMeansMPI_DeterministicRequestInstance;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER_RANK) {
        start_time = MPI_Wtime();
    }
    
    if (argc != 4) {
        printf("A quantidade de argumentos é inválida!\n");
        exit(0);
    }
    K = atoi(argv[3]);
    if (rank == MASTER_RANK) {
        read_start_time = MPI_Wtime();
    }
    database = BinDatabaseReader_ReadMPI(argv[1], atoi(argv[2]));
    if (rank == MASTER_RANK) {
        read_end_time = MPI_Wtime();
    }
    KMeansMPI_CompStartCentroids(K);
    func_obj_line_local = KMeans_CompFuncObj(K);

    // Reduz o somatório da função objetivo para todos processos.
    MPI_Allreduce(&func_obj_line_local, &func_obj_line_global, 1, MPI_DOUBLE, 
        MPI_SUM, MPI_COMM_WORLD);
    // if (rank == MASTER_RANK) {
    //     printf("OBJ(%d) -> %lf\n", count_it, func_obj_line_global);
    //     // KMeans_PrintCentroids(K);
    // }
    do {
        func_obj = func_obj_line_global;
        KMeansMPI_RecalcClusterCentroids(K);
        func_obj_line_local = KMeans_CompFuncObj(K);
        func_obj_line_global = 0;
        // Reduz o somatório da função objetivo para todos processos.
        MPI_Allreduce(&func_obj_line_local, &func_obj_line_global, 1, MPI_DOUBLE, 
            MPI_SUM, MPI_COMM_WORLD);
        count_it++;
        // if (rank == MASTER_RANK) {
        //     printf("OBJ(%d) -> %lf\n", count_it, func_obj_line_global);
        //     // KMeans_PrintCentroids(K);
        // }
    } while (func_obj - func_obj_line_global > THRESHOLD);
    
    // if (rank == MASTER_RANK) {
    //     printf("func_obj -> %lf (%d)\n", func_obj_line_global, rank);
    //     printf("QTD iteracoes: %d\n", count_it);
    //     printf("Centroids finais:\n");
    //     KMeans_PrintCentroids(K);
    //     printf("database->instances->used -> %ld\n", database->instances->used);
    //     printf("database->features_length -> %d\n", database->features_length);
    // }

    if (rank == MASTER_RANK) {
        write_start_time = MPI_Wtime();
        KMeansMPI_WriteReport(argv[1], K);
        end_time = MPI_Wtime();
        long time_total = (end_time - start_time) * 1000000;
        long time_read = (read_end_time - read_start_time) * 1000000;
        long time_write = (end_time - write_start_time) * 1000000;
        long time_kmeans = time_total - (time_write + time_read);

        printf("%ld,%ld,%ld,%ld", time_read, time_write, time_kmeans, time_total);
        // printf("Tempo total      ->   %ldus\n", time_total);
        // printf("Tempo de leitura ->   %ldus\n", time_read);
        // printf("Tempo de escrita ->   %ldus\n", time_write);
        // printf("Tempo algoritmo  ->   %ldus\n", time_total - (time_write + time_read));
    } else {
        KMeansMPI_ListenClusterRequest();
    }

    return 0;
}