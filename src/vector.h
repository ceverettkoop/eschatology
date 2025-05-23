#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include "error.h"

#define PAGE_SIZE 4096

#define VEC_GET(vector, type, index) \
    (((size_t)(index) >= (vector).size) ? \
        (err_vector_index_out_of_bounds((size_t)(index), (vector).size), \
         *((type*)NULL)) : \
        ((type*)(vector).data)[index])

typedef struct vector_tag Vector;

struct vector_tag{
    size_t page_count;
    size_t element_size;
    size_t size;
    void *data;
};

Vector new_vector(size_t element_size);
int vec_contains(Vector *p, void *value);
void vec_push_back(Vector* ptr, void* element, size_t n);
void vec_clear(Vector *ptr);
void free_vec(Vector *ptr);


#endif /* VECTOR_H */
