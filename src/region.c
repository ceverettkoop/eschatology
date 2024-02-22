#include "region.h"
#include "error.h"
#include "time.h"
#include <stdlib.h>

static void blank_with_grass(Region *p);
static void generate_boundaries(Region *p);
static void gen_straight_tile_line(Tile* origin, bool is_x_axis, int line_len, Tile tile);
static void gen_rand_tile_line(Tile *origin, bool is_x_axis, int extent, int min_entity_count, int max_entity_count, 
    Tile tile);

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
    generate_boundaries(ret_ptr); //includes exits

    return ret_ptr;
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
    for (int i = 0; i < ROWS; i++){
        for (int n = 0; n < COLUMNS; n++){
            p->tile_matrix[i][n] = get_grass_tile();
        }
    }
}

static void generate_boundaries(Region *p){
    //north
    gen_straight_tile_line(&(p->tile_matrix[0][0]), true, REGION_WIDTH, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][0]), true, REGION_WIDTH, 1, 6, get_grass_tile());
    //west
    gen_straight_tile_line(&(p->tile_matrix[0][0]), false, REGION_HEIGHT, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][0]), false, REGION_HEIGHT, 1, 6, get_grass_tile());
    //south
    gen_straight_tile_line(&(p->tile_matrix[ROWS - 1][0]), true, REGION_WIDTH, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[ROWS - 1][0]), true, REGION_WIDTH, 1, 6, get_grass_tile());
    //east
    gen_straight_tile_line(&(p->tile_matrix[0][COLUMNS - 1]), false, REGION_HEIGHT, get_mountain_tile());
    gen_rand_tile_line(&(p->tile_matrix[0][COLUMNS - 1]), false, REGION_HEIGHT, 1, 6, get_grass_tile());
}

static void gen_straight_tile_line(Tile* origin, bool is_x_axis, int line_len, Tile tile){
    if(is_x_axis){
        for (int i = 0; i < line_len; i++){
            *(origin + i) = tile;
        }
    }else{
        for (int i = 0; i < line_len; i++){
            *(origin + (i * COLUMNS)) = tile;
        } 
    }
}

//populates between 0 and max_entity_count elements along line
static void gen_rand_tile_line(Tile *origin, bool is_x_axis, int extent, int min_entity_count, 
                            int max_entity_count, Tile tile){

    const int count = rand()/(RAND_MAX / (max_entity_count - min_entity_count) ) + min_entity_count;
    for (int i = 0; i < count; i++){
        int offset = rand()/(RAND_MAX / extent);
        if(is_x_axis){
            *(origin + offset) = tile;
        }else{
            *(origin + offset * COLUMNS) = tile;
        }
    }
}