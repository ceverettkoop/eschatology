#include "region.h"
#include "error.h"
#include <stdlib.h>

static void blank_with_grass(Region *p);
static void generate_boundaries(Region *p);

Region *generate_region(){
    Region *ret_ptr = malloc(sizeof(Region));
    check_malloc(ret_ptr);

    //set neighbors to null
    ret_ptr->north = NULL;
    ret_ptr->south = NULL;
    ret_ptr->west = NULL;
    ret_ptr->east = NULL;

    //new seed
    srand(time(NULL));

    //actually create region
    blank_with_grass(ret_ptr);
    generate_boundaries(ret_ptr);

    //qty of exits 
    
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

static void blank_with_grass(Region *p){
    for (int i = 0; i < REGION_WIDTH; i++){
        for (int n = 0; n < REGION_HEIGHT; n++){
            p->tile_matrix[i][n] = get_grass_tile();
        }
    }
}

static void generate_boundaries(Region *p){
    //0-5 entries
    const int north_count = rand()/(RAND_MAX / 6);


}