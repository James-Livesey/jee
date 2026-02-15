#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <stdbool.h>

#include "errno.h"

typedef struct Vector {
    size_t elem_size;
    size_t length;
    void* elems;
} Vector;

typedef int (*vector_sorter_t)(const void* a, const void* b);

bool new_vector(size_t elem_size, Vector* vector);
void* vector_get(const Vector* vector, size_t index);
size_t vector_get_index(const Vector* vector, const void* elem);
bool vector_set(Vector* vector, size_t index, const void* elem);
size_t vector_push(Vector* vector, const void* elem);
bool vector_remove_index(Vector* vector, size_t index);
bool vector_remove(Vector* vector, const void* elem);
void vector_sort(Vector* vector, vector_sorter_t sorter);

#endif