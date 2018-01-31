# k-means-mpi

## Introdução

Implementação do algoritmo de clusterização _k-means_ paralelo utilizando a biblioteca MPI (Message Passing Interface) na linguagem de programação ANSI C.

## Compilando o projeto

Para compilar o projeto utilize o comando:
```bash
make
```

São gerados 3 executáveis no diretório bin:
* ```bin_database_writer``` - converter uma base de dados em csv em uma base de dados binária
* ```kmeans_mpi_app``` - implementação multi-processos do algoritmo _k-means_ usando  com a biblioteca MPI 
* ```kmeans_seq_app``` - implementação sequencial do algoritmo _k-means_

## Gerando base de dados binária

Para gerar uma base de dados binária com a aplicação _bin_database_writer_, execute-a da seguinte forma:
```bash
./bin_database_writer <path_database> <class_column>
```
onde:
* ```<path_database>``` - é o caminho onde a base de dados csv está salva
* ```<class_column>``` - é um inteiro que indica o índice da coluna onde a classe da instância está localizada

*LIMITAÇÃO* - Essa aplicação só funciona para base de dados onde todas _features_ das instâncias são numéricas, exceto a classe.

## Executando o k-means

A aplicação _kmeans_mpi_app_ executa o _k-means_ com multiprocessos usando MPI. Para executar essa aplicação utilize o comando abaixo:

```bash
mpirun -np <num_process> ./kmeans_mpi_app <path_database_bin> <num_features> <k> <debug_mode>
```
onde:
* ```<num_process>``` - quantidade de processos que a aplicação deve utilizar para executar
* ```<path_database_bin>``` - caminho onde a base de dados binária está salva
* ```<num_features>``` - quantidade de _features_ que as instâncias da base de dados possui
* ```<k>```  - quantidade de clusters em que o algoritmo deve agrupar as instâncias
* ```<debug_mode>``` - parâmetro opcional. Valores válidos para esse parâmetro são "-d" ou "--debug". Se executar com este parâmetro, executará no modo de debug, onde a cada iteração irá mostrar os centroides atuais e a função objetivo.

Essa aplicação irá gerar um relatório no arquivo com o nome '<path_database_bin>.clusters' que contém informações sobre os clusters gerados pela aplicação, onde cada cluster terá o seu centroide e também os índices das instâncias que estão contidas nesse cluster.

A aplicação _kmeans_seq_app_ executa o _k-means_ de forma sequencial. Para executar essa aplicação utilize o comando abaixo:

```bash
./kmeans_seq_app <path_database> <class_column> <k> <debug_mode>
```
onde:
* ```<path_database>``` - é o caminho onde a base de dados csv está salva
* ```<class_column>``` - é um inteiro que indica o índice da coluna onde a classe da instância está localizada
* ```<k>```  - quantidade de clusters em que o algoritmo deve agrupar as instâncias
* ```<debug_mode>``` - parâmetro opcional. Valores válidos para esse parâmetro são "-d" ou "--debug". Se executar com este parâmetro, executará no modo de debug, onde a cada iteração irá mostrar os centroides atuais e a função objetivo.

Essa aplicação irá gerar um relatório no arquivo com o nome '<path_database>.clusters' que contém informações sobre os clusters gerados pela aplicação, onde cada cluster terá o seu centroide e também os índices das instâncias que estão contidas nesse cluster.

## Outras aplicações do projeto

* ```install_mpi_and_test.sh``` - instalação do MPI e teste para verificação se a instalação foi realizada corretamente
* ```install_python3_pip_and_libs.sh``` - instalação do python3 e bibliotecas utilizadas nos scripts python implementados nesse projeto
* ```expand_database.py``` - possui funções para facilitar a expansão de uma base de dados em csv. No arquivo possui duas funções com exemplos de uso para expansão da base de dados, essas funções são: _expandSegmentationDatabase_ e _expandLetterDatabase_.
* ```correction_evaluation.sh``` - script utilizado para verificar a corretude do algoritmo _k-means_ multiprocessos. Esse script executa o _k-means_ sequencial e paralelo para verificar se o resultado é o mesmo.
* ```exec_script.sh``` - script utilizado para realizar testes com várias bases de dados e vários números de processos. O _header_ desse arquivo contém os parâmetros do script a ser executado.
* ```process_report.py``` - processa os dados de um experimento para conseguir os dados estatísticos em uma nova planilha geral do experimento e também gera os gráficos do experimento.

## Testes e relatórios gerados

O diretório _reports_gcloud_ possui relatórios de testes executados com quatro bases de dados em uma máquina na Google Cloud com 24 cores e 24GB de memória RAM. Mais informações da máquina estão no arquivo _cpu_server_machine.info. As bases de dados utilizadas estão no diretório databases e são as bases segmentation e letter-recognition, as outras duas bases de dados utilizadas são versões dez vezes maiores dessas duas bases de dados, e foram gerados com a aplicação em python  _expand_database.py_. No relatório é possível ver informações sobre quatro tipos de medidas de tempo: tempo gasto para realizar a leitura da base de dados (_time_in_), tempo gasto para executar o _k-means_ (_time_kmeans_), tempo gasto para escrever o relatório da aplicação (_time_report_), e por último o tempo geral (_time_total_). Com os dados brutos foram criadas planilhas com dados estatísticos, esses dados são: o tempo médio dos testes realizados em um cenário, o seu intervalo de confiança para um grau de confiança de 95%, o _speedup_, e a eficiência.