#ifndef _ARRAY_
#define _ARRAY_

#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    void** data;
    size_t used;
    size_t size;
} S_Array;

typedef S_Array* Array;

Array makeArray(size_t initial_size);
void addArray(Array array, void* element);
void* getArray(Array array, int index);
void freeArray(Array array);
void deepFreeArray(Array array, void (*free_element)(void*));

#endif /*_ARRAY_*/

