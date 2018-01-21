#ifndef _ARRAY_
#define _ARRAY_

#include <stdio.h>

/**
 * Multiplicador de crescimento do array redimensionável.
 * */
#define GROWTH_FACTOR 2

/**
 * Representa um array redimensionável genérico. Onde, data é o array de ponteiros de 
 * void (elementos genéricos), used é a quantidade do array que está sendo usada, e size
 * é o tamanho atual do array.
 * */
typedef struct {
    void** data;
    size_t used;
    size_t size;
} S_ArrayList;

/**
 * Definição do tipo ponteiro de lista usando array redimensionável.
 * */
typedef S_ArrayList* ArrayList;

/**
 * Cria um lista usando um array redimensionável de tamanho inicial igual 
 * a initial_size;
 * 
 * @param initial_size tamanho inicial do array redimensionável
 * 
 * @return array redimensionável criado
 * */
ArrayList ArrayList_Create(size_t initial_size);

/**
 * Adiciona um elemento no final da lista.
 * 
 * @param array_list lista, onde elemento será adicionado
 * @param element elemento a ser adicionado no lista
 * */
void ArrayList_Add(ArrayList array_list, void* element);

/**
 * Recupera elemento que está na posição index da lista.
 * 
 * @param array_list lista, onde irá recuperar elemento
 * @param index índice do elemento a ser recuperado
 * 
 * @return elemento recuperado
 * */
void* ArrayList_Get(ArrayList array_list, int index);

/**
 * Libera a memória alocada para a estrutura de uma determinada lista 
 * implementado por um array redimensionável.
 * 
 * @param array_list lista em que a memória será liberada.
 * */
void ArrayList_Destroy(ArrayList array_list);

/**
 * Libera a memória alocada para a estrutura de uma determinada lista, implementada 
 * por um array redimensionável, e também libera a memória usada para alocar seus 
 * elementos.
 * 
 * @param array_list lista em que a memória será liberada.
 * @param free_element função que libera a memória dos elementos contidos na lista.
 * */
void ArrayList_DestroyDeeply(ArrayList array_list, void (*free_element)(void*));

#endif /*_ARRAY_*/