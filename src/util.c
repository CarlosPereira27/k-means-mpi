#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h>
#include <util.h>


/**
 * Verifica se um determinado array contém um determinado valor.
 * 
 * @param array array a ser verificada presença de valor
 * @param length tamanho do array
 * @param value valor a ser verificado se está contido no array
 * 
 * @return TRUE se o valor está contido no array, caso contrário, FALSE. 
 * */
BOOLEAN Util_ContainsValue(int* array, int length, int value) {
    int i;

    for (i = 0; i < length; i++) {
        if (array[i] == value) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Gera um número inteiro aleatório entre 0 e um limitante superior.
 * 
 * @param max limitante superior
 * 
 * @return número inteiro aleatório 
 * */
int Util_GenerateRandomIntValue(int max) {
    return rand() % max;
}

/**
 * Gera K números inteiros de forma aleatória entre 0 e um limitante superior.
 * 
 * @param K quantidade de números
 * @param max limitante superior
 * 
 * @return vetor com K números inteiros
 * */
int* Util_GenerateRandomKIntValues(int K, int max) {
    int* indexes = (int*) malloc(K * sizeof(int));
    int i;

    for (i = 0; i < K; i++) {
        int value = Util_GenerateRandomIntValue(max);
        while (Util_ContainsValue(indexes, i, value)) {
            value = Util_GenerateRandomIntValue(max);
        }
        indexes[i] = value;
    }

    return indexes;
}

/**
 * Gera K números inteiros de forma determinística entre 0 e um 
 * limitante superior.
 * 
 * @param K quantidade de números
 * @param max limitante superior
 * 
 * @return vetor com K números inteiros
 * */
int* Util_GenerateDeterministicKIntValues(int K, int length) {
    int* indexes = (int*) malloc(K * sizeof(int));
    int i;

    for (i = 0; i < K; i++) {
        indexes[i] = i;
    }

    return indexes;
}

/**
 * Calcula o quadrado da distância entre dois pontos de dimensão N.
 * 
 * @param point_1 primeiro ponto
 * @param point_2 segundo ponto
 * @param N dimensão dos pontos
 * 
 * @return quadrado da distância entre os dois pontos
 * */
double Util_SquareDistance(double* point_1, double* point_2, int N) {
    int i;
    double sq_distance = 0.0;

    for (i = 0; i < N; i++) {
        double dif = point_1[i] - point_2[i];
        sq_distance += dif * dif;
    }

    return sq_distance;
}

/**
 * Define um valor de memória em uma matriz.
 * 
 * @param matrix matriz a ser definida o valor
 * @param value valor a ser definido na memória da matriz
 * @param num_lines quantidade de linhas da matriz
 * @param num_bytes_per_line quantidade de bytes por linha da matriz
 * */
void Util_MemsetMatrix(void** matrix, int value, int num_lines, size_t num_bytes_per_line) {
    int i;
    for (i = 0; i < num_lines; i++) {
        memset(matrix[i], value, num_bytes_per_line);
    }
}

/**
 * Realiza um calloc para cada linha de uma matriz.
 * 
 * @param matrix matriz a ser realizado o calloc
 * @param num_lines quantidade de linhas da matriz
 * @param num_elements_per_line quantidade de elementos por linha da matriz
 * @param elements_size tamanho de cada elemento em bytes
 * */
void Util_CallocMatrix(void** matrix, int num_lines, int num_elements_per_line, 
        size_t elements_size) {
    int i;
    for (i = 0; i < num_lines; i++) {
        matrix[i] = calloc(num_elements_per_line, elements_size);
    }
}

/**
 * Imprime um ponto na saída padrão.
 * 
 * @param point ponto a ser impresso
 * @param length dimensão do ponto a ser impresso
 * */
void Util_PrintPoint(double* point, int length) {
    int i = 0;
    printf("%lf", point[i]);
    for (i++; i < length; i++) {
        printf(",%lf", point[i]);
    }
}


/**
 * Escreve um ponto em um arquivo.
 * 
 * @param file arquivo em que o ponto será escrito
 * @param point ponto a ser impresso
 * @param length dimensão do ponto a ser escrito
 * */
void Util_WritePoint(FILE* file, double* point, int length) {
    int i = 0;

    fprintf(file, "%lf", point[i]);
    for (i++; i < length; i++) {
        fprintf(file, ",%lf", point[i]);
    }
}