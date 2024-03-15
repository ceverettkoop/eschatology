#include <stdlib.h>
#include "sprite.h"
#include "entity.h"
#include "gamestate.h"

//this could be a macro
void add_sprite(EntityID id, Sprite value, GameState* gs) {
    void *sprite_ptr = malloc(sizeof(Sprite));
    check_malloc(sprite_ptr);

    void *old_val = sc_map_put_64v(gs->sprite_map, id, sprite_ptr);
    if(sc_map_found(gs->sprite_map)){
        free(old_val);
        err_component_exists("Sprite");
    }
    *((Sprite*)sprite_ptr) = value;
}

void free_sprite(EntityID id, GameState *gs) {
    void *component_to_free = sc_map_get_64v(gs->sprite_map, id);
    if(sc_map_found(gs->sprite_map)){
        free(component_to_free);
        sc_map_del_64v(gs->sprite_map, id);
    }else{
        err_free_missing_component("Sprite");
    }
}