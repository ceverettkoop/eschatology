#include "region.h"
#include "error.h"
#include <stdlib.h>

Region *generate_region(){
    Region *ret_ptr = malloc(sizeof(Region));
    check_malloc(ret_ptr);

    //set neighbors to null
    ret_ptr->north = NULL;
    ret_ptr->south = NULL;
    ret_ptr->west = NULL;
    ret_ptr->east = NULL;

    //get seed for random generation
    srand(time(NULL));
    int seed = rand();

    //actually create region

}

void generate_neighbors(Region *region_ptr){
    if(region_ptr->north == NULL){
        region_ptr->north = generate_region();
        region_ptr->north->south = region_ptr;
    }
    if(region_ptr->south == NULL){
        region_ptr->south = generate_region();
        region_ptr->south->north = region_ptr;
    }
    if(region_ptr->west == NULL){
        region_ptr->west = generate_region();
        region_ptr->west->east = region_ptr;
    }
    if(region_ptr->east == NULL){
        region_ptr->east = generate_region();
        region_ptr->east->west = region_ptr;
    }
}
