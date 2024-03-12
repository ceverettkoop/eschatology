#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdint.h>
#include "region.h"
#include "entity.h"

typedef struct EntityNode_tag{
    EntityID id;
    struct EntityNode_tag *next;
    struct EntityNode_tag *prev;
}EntityNode;

typedef struct GameState_tag{
    Region *cur_region_ptr;
    EntityID next_id;
    EntityNode *entity_list_head;
    EntityNode *entity_list_back;
} GameState;

void gs_init(GameState *gs);
void update_gamestate(GameState *gs, UserInput input);

EntityID new_entity(GameState *gs);
void free_entity(EntityID id, GameState *gs);

#endif /* GAMESTATE_H */
