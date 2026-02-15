#include <stdio.h>
#include <string.h>

#include "vector.h"

bool new_vector(size_t elem_size, Vector* vector) {
    vector->elem_size = elem_size;
    vector->length = 0;
    vector->elems = malloc(0);

    return vector->elems;
}

void* vector_get(const Vector* vector, size_t index) {
    if (index >= vector->length) {
        return NULL;
    }

    return vector->elems + (index * vector->elem_size);
}

size_t vector_get_index(const Vector* vector, const void* elem) {
    for (size_t i = 0; i < vector->length; i++) {
        if (memcmp(vector->elems + (i * vector->elem_size), elem, vector->elem_size) == 0) {
            return i;
        }
    }

    return -1;
}

bool vector_set(Vector* vector, size_t index, const void* elem) {
    if (index >= vector->length) {
        return false;
    }

    memcpy(vector->elems + (index * vector->elem_size), elem, vector->elem_size);

    return true;
}

size_t vector_push(Vector* vector, const void* elem) {
    size_t new_length = vector->length + 1;
    void* new_elems = realloc(vector->elems, new_length * vector->elem_size);

    if (!new_elems) {
        return -1;
    }

    vector->elems = new_elems;

    memcpy(vector->elems + (vector->length * vector->elem_size), elem, vector->elem_size);

    printf("vl %d new %d\n", vector->length, new_length);

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

bool vector_remove(Vector* vector, const void* elem) {
    size_t index = vector_get_index(vector, elem);

    if (index == -1) {
        return false;
    }

    return vector_remove_index(vector, index);
}

void vector_swap(Vector* vector, size_t a_index, size_t b_index) {
    char* a = vector_get(vector, a_index);
    char* b = vector_get(vector, b_index);

    for (size_t i = 0; i < vector->elem_size; i++) {
        char temp = a[i];

        a[i] = b[i];
        b[i] = temp;
    }
}

void vector_quicksort(Vector* vector, vector_sorter_t sorter, size_t first_index, size_t last_index) {
    size_t pivot;
    size_t i;
    size_t j;

    if ((int)first_index >= (int)last_index) {
        return;
    }

    pivot = first_index;
    i = first_index;
    j = last_index;

    while (i < j) {
        while (sorter(vector_get(vector, i), vector_get(vector, pivot)) <= 0 && i < last_index) {
            i++;
        }
        
        while (sorter(vector_get(vector, j), vector_get(vector, pivot)) > 0) {
            j--;
        }
        
        if (i < j) {
            vector_swap(vector, i, j);
        }
        
        vector_swap(vector, pivot, j);
        
        vector_quicksort(vector, sorter, first_index, j - 1);
        vector_quicksort(vector, sorter, j + 1, last_index);
    }
}

void vector_sort(Vector* vector, vector_sorter_t sorter) {
    vector_quicksort(vector, sorter, 0, vector->length - 1);
}