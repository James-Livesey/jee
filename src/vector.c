#include <string.h>

#include "vector.h"

bool new_vector(size_t elem_size, Vector* vector) {
    vector->elem_size = elem_size;
    vector->length = 0;
    vector->elems = malloc(0);

    return vector->elems;
}

void* vector_get(Vector* vector, size_t index) {
    if (index >= vector->length) {
        return NULL;
    }

    return vector->elems + (index * vector->elem_size);
}

size_t vector_get_index(Vector* vector, void* elem) {
    for (size_t i = 0; i < vector->length; i++) {
        if (memcmp(vector->elems + (i * vector->elem_size), elem, vector->elem_size) == 0) {
            return i;
        }
    }

    return -1;
}

size_t vector_push(Vector* vector, void* elem) {
    size_t new_length = vector->length + 1;
    void* new_elems = realloc(vector->elems, new_length * vector->elem_size);

    if (!new_elems) {
        return -1;
    }

    vector->elems = new_elems;

    void* vectored_elem = vector->elems + vector->length;

    memcpy(vectored_elem, elem, vector->elem_size);

    vector->length = new_length;

    return vector->length;
}

bool vector_remove_index(Vector* vector, size_t index) {
    if (index >= vector->length) {
        return false;
    }

    void* removed_elem = vector_get(vector, index);

    memcpy(removed_elem, removed_elem + vector->elem_size, (vector->length - index - 1) * vector->elem_size);

    void* new_elems = realloc(vector->elems, --vector->length * vector->elem_size);

    if (!new_elems && vector->length != 0) {
        return false;
    }

    vector->elems = new_elems;

    return true;
}

bool vector_remove(Vector* vector, void* elem) {
    size_t index = vector_get_index(vector, elem);

    if (index == -1) {
        return false;
    }

    return vector_remove_index(vector, index);
}