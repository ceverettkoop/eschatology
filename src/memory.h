#ifndef MEMORY_H
#define MEMORY_H

#include "stdlib.h"

//way to define this based on implementation??
#define MEMORY_PAGE 4000

#define page_alloc(allocator, type) \
    allocator.page_count = 0; \
    while(sizeof(type) > MEMORY_PAGE * allocator.page_count ){ allocator.page_count++;} \
    allocator.ptr = malloc( MEMORY_PAGE * allocator.page_count);
 
#define page_realloc(allocator, type, new_count) \
    if(sizeof(type) * new_count > allocator.page_count * MEMORY_PAGE){ \
        while(sizeof(type) * new_count > allocator.page_count * MEMORY_PAGE){ \
            allocator.page_count++; \
        } \
        allocator.ptr = realloc(allocator.ptr, MEMORY_PAGE * allocator.page_count); \
    }

#define page_free(allocator) free(allocator.ptr);

typedef struct Allocator_tag{
    void *ptr;
    size_t page_count;
}Allocator;

#endif /* MEMORY_H */
