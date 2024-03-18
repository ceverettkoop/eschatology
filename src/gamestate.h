#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>
#include "entity.h"
#include "user_input.h"
#include "sc_map.h"

struct Region_tag;

typedef struct EntityNode_tag{
    EntityID id;
    struct EntityNode_tag *next;
    struct EntityNode_tag *prev;
}EntityNode;

typedef struct GameState_tag{
    struct Region_tag *cur_region_ptr;
    EntityID next_id;
    EntityNode *entity_list_head;
    EntityNode *entity_list_back;
    struct sc_map_64v Position_map;
    struct sc_map_64v Sprite_map;
    struct sc_map_64v Tile_map;
    struct sc_map_64v Region_map;
} GameState;

void gs_init(GameState *gs);
void gs_deinit(GameState *gs);
void update_gamestate(GameState *gs, UserInput input);

EntityID new_entity(GameState *gs);
void free_entity(EntityID id, GameState *gs);

#endif /* GAMESTATE_H */
