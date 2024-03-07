#include "gamestate.h"
#include "avatar.h"
#include "error.h"
#include <stdlib.h>

static EntityNode *generate_head_entity(GameState *gs);

void gs_init(GameState *gs){
    //region
    gs->cur_region_ptr = generate_region();
    generate_neighbors(gs->cur_region_ptr);

    //player
    gs->entity_head = generate_head_entity(gs);

    //addl entities

}

void update_gamestate(GameState *gs){


}

static EntityNode *generate_head_entity(GameState *gs){
    gs->entity_head = malloc(sizeof(EntityNode));
    check_malloc(gs->entity_head);
    gs->entity_head->prev = NULL;
    gs->entity_head->next = NULL;
    gs->entity_head->ptr = init_player_avatar(gs->cur_region_ptr);

}