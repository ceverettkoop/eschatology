#include "vector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

Vector new_vector(size_t element_size) {
    Vector ret_val;
    ret_val.size = 0;
    ret_val.element_size = element_size;
    ret_val.page_count = 1;
    while (element_size > PAGE_SIZE * ret_val.page_count) {
        ret_val.page_count++;
    }
    ret_val.data = malloc(PAGE_SIZE * ret_val.page_count);
    check_malloc(ret_val.data);
    return ret_val;
}

int vec_contains(Vector* p, void* value) { 
    void *cur = p->data;
    int offset = 0;
    while(offset != p->element_size * p->size){
        if(memcmp(cur, value, p->element_size) == 0){
            return 1;
        }
        cur += p->element_size;
        offset += p->element_size;
    }
    return 0;
}

void vec_push_back(Vector* ptr, void* element, size_t n) {
    bool resize = false;
    size_t offset = 0;
    ptr->size = ptr->size + n;
    while (ptr->element_size * ptr->size > PAGE_SIZE * ptr->page_count) {
        resize = true;
        ptr->page_count++;
    }
    if (resize) {
        ptr->data = realloc(ptr->data, PAGE_SIZE * ptr->page_count);
        check_malloc(ptr->data);
    }
    offset = (ptr->size - n) * ptr->element_size;
    memcpy(ptr->data + offset, element, n);
}

void vec_clear(Vector* ptr) {
    free(ptr->data);
    *ptr = new_vector(ptr->element_size);
}

void free_vec(Vector* ptr) { free(ptr->data); }
