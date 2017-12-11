#include <string.h>
#include "array.h"
#include "database_handler.h"


Array makeArray(size_t initial_size) {
    Array array = (Array) malloc(sizeof(S_Array));
    array->data = (void**) malloc(initial_size * sizeof(void*));
    array->used = 0;
    array->size = initial_size;
    return array;
}

void addArray(Array array, void* element) {
    if (array->used == array->size) {
        array->size *= 2;
        array->data = (void**) realloc(array->data, array->size * sizeof(void*));
    }
    array->data[array->used++] = element;
}

void* getArray(Array array, int index) {
    return array->data[index];
}

void freeArray(Array array) {
    free(array->data);
    free(array);
}

void deepFreeArray(Array array, void (*free_element)(void*)) {
    size_t i = 0;
    if (!free_element) {
        i = array->used;
    }
    for (i = 0; i < array->used; i++) {
        free_element(array->data[i]);
    }
    free(array->data);
    free(array);
}

