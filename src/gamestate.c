#include <stdlib.h>
#include "gamestate.h"
#include "error.h"
#include "sc_map.h"
#include "components/region.h"

static inline EntityID init_entity_list(GameState *gs);

void gs_init(GameState *gs) {
    EntityID player_entity_id;
    EntityID init_region_id;

    //TODO MAKE THIS A MACRO
    // init hashmaps for all components
    sc_map_init_64v(&gs->Position_map,0,0);
    sc_map_init_64v(&gs->Sprite_map,0,0);
    sc_map_init_64v(&gs->Tile_map,0,0);
    sc_map_init_64v(&gs->Region_map,0,0);
    sc_map_init_64v(&gs->Name_map,0,0);

    player_entity_id = init_entity_list(gs);
    // region
    gs->cur_region_ptr = generate_region(gs, &init_region_id);
    generate_neighbors(init_region_id, gs);

    // init player
    //init_player(player_entity_id);
}

void update_gamestate(GameState *gs, UserInput input) {


}

// assume head and back always non null... at least one entity
EntityID new_entity(GameState *gs) {
    EntityID id_to_issue = gs->next_id;
    EntityNode *new_node = malloc(sizeof(EntityNode));
    check_malloc(new_node);
    new_node->id = id_to_issue;
    gs->entity_list_back->next = new_node;
    new_node->prev = gs->entity_list_back;
    new_node->next = NULL;
    gs->entity_list_back = new_node;

    // tick up and bail on overflow
    if (gs->next_id == INT64_MAX) err_overflow();
    gs->next_id++;

    return id_to_issue;
}

void free_entity(EntityID id, GameState *gs){
    //TODO locate every component reference to this entity
    //will make hashmaps for every system
    EntityNode *cur = gs->entity_list_head;
    while(cur != NULL){
        if(cur->id == id) break;
        cur = cur->next;
    }
    if(cur == NULL) err_entity_not_found();
    if(cur == gs->entity_list_head) err_free_list_head();  
    //we know it's not the head
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    free(cur);
}

static inline EntityID init_entity_list(GameState *gs) {
    EntityID init_id = 0;

    // one time init of list head
    gs->entity_list_head = malloc(sizeof(EntityNode));
    check_malloc(gs->entity_list_head);

    gs->entity_list_head->id = init_id;
    gs->entity_list_head->prev = NULL;
    gs->entity_list_head->next = NULL;

    gs->next_id = init_id + 1;

    gs->entity_list_back = gs->entity_list_head;

    return init_id;
}