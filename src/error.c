#include "error.h"
#include "stdlib.h"
#include "stdio.h"

void check_malloc(void *ptr){
    if(ptr == NULL){
        fprintf(stderr,"Error: out of memory\n");
        exit(1);
    }
}