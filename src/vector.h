#include <stdlib.h>
#include <stdbool.h>

#include "errno.h"

typedef struct Vector {
    size_t elem_size;
    size_t length;
    void* elems;
} Vector;

bool new_vector(size_t elem_size, Vector* vector);
void* vector_get(Vector* vector, size_t index);
size_t vector_get_index(Vector* vector, void* elem);
size_t vector_push(Vector* vector, void* elem);
bool vector_remove_index(Vector* vector, size_t index);
bool vector_remove(Vector* vector, void* elem);