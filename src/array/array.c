
#include <array/array.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>


/**
 * Cria um lista usando um array redimensionável de tamanho inicial igual 
 * a initial_size;
 * 
 * @param initial_size
 *      tamanho inicial do array redimensionável
 * 
 * @return array
 *      redimensionável criado
 * */
ArrayList ArrayList_Create(size_t initial_size) {
    ArrayList array_list = (ArrayList) malloc(sizeof(S_ArrayList));

    array_list->data = (void**) malloc(initial_size * sizeof(void*));
    array_list->used = 0;
    array_list->size = initial_size;
    
    return array_list;
}

/**
 * Adiciona um elemento no final da lista.
 * 
 * @param array_list
 *      lista, onde elemento será adicionado
 * @param element
 *      elemento a ser adicionado no lista
 * */
void ArrayList_Add(ArrayList array_list, void* element) {
    if (array_list->used == array_list->size) {
        array_list->size *= GROWTH_FACTOR;
        array_list->data = (void**) realloc(array_list->data, array_list->size * sizeof(void*));
    }
    array_list->data[array_list->used++] = element;
}

/**
 * Recupera elemento que está na posição index da lista.
 * 
 * @param array_list
 *      lista, onde irá recuperar elemento
 * @param index
 *      índice do elemento a ser recuperado
 * 
 * @return elemento recuperado
 * */
void* ArrayList_Get(ArrayList array_list, int index) {
    return array_list->data[index];
}

/**
 * Libera a memória alocada para a estrutura de uma determinada lista 
 * implementado por um array redimensionável.
 * 
 * @param array_list
 *      lista em que a memória será liberada.
 * */
void ArrayList_Destroy(ArrayList array_list) {
    free(array_list->data);
    free(array_list);
}

/**
 * Libera a memória alocada para a estrutura de uma determinada lista, implementada 
 * por um array redimensionável, e também libera a memória usada para alocar seus 
 * elementos.
 * 
 * @param array_list
 *      lista em que a memória será liberada.
 * @param free_element
 *      função que libera a memória dos elementos contidos na lista.
 * */
void ArrayList_DestroyDeeply(ArrayList array_list, void (*free_element)(void*)) {
    size_t i = 0;
    if (array_list == NULL)
        return;
    if (free_element == NULL)
        i = array_list->used;
    if (array_list->data != NULL) {
        for (i = 0; i < array_list->used; i++)
            free_element(array_list->data[i]);
        free(array_list->data);
    }
    free(array_list);
}