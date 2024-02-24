#include "entity.h"
#include "error.h"
#include "region.h"
#include <stdlib.h>

EntityNode *new_entity(EntityNode *list_head, SpriteID _sprite,
    Region *_region_ptr, TilePos _pos, EntityType type){

    EntityNode *cur = list_head;
    while(cur != NULL){
        cur = cur->next;
    }
    EntityNode *back = cur;
    cur = malloc(sizeof(EntityNode));
    if(back != NULL){
        back->next = cur;
    }
    cur->prev = back;
    cur->next = NULL;
    cur->ptr = malloc(sizeof(Entity));
    check_malloc(cur->ptr);

    //DONT KNOW HOW TO DO THIS
    cur->ptr->properties_ptr = prop_init(type, &cur->ptr->is_mobile);
    //POINT TO A

    cur->ptr->sprite = _sprite;
    cur->ptr->region_ptr = _region_ptr;
    cur->ptr->pos = _pos;
}


void free_entity(EntityNode *ptr){
    if(ptr->next != NULL){
        ptr->next->prev = ptr->prev;
    }
    if(ptr->prev != NULL){
        ptr->prev->next = ptr->next;
    }
    free(ptr->ptr->properties_ptr);
    free(ptr->ptr);
    free(ptr);
}