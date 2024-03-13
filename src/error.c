#include "error.h"
#include "stdlib.h"
#include "stdio.h"

void check_malloc(void *ptr){
    if(ptr == NULL){
        fprintf(stderr,"Error: out of memory\n");
        exit(1);
    }
}

void err_overflow(){
    fprintf(stderr, "Error: integer overflow\n");
    exit(1);
}

void err_entity_not_found() {
    fprintf(stderr, "Error: entity not found\n");
    exit(1);
}

void err_free_list_head() {
    fprintf(stderr, "Error: cannot remove head entity/player\n");
    exit(1);    
}

void err_component_exists(const char *component_str) {
    fprintf(stderr, "Warning: attempt to write component of type %s where already exists\n", component_str);
}

void err_no_sprite_to_draw(int row, int column) {
    fprintf(stderr, "Error: no sprite to draw at row %d and column %d\n");
    exit(1);
}
