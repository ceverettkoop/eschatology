#ifndef GAMESTATE_H
#define GAMESTATE_H

#define GAME_MAX_ENTITIES 2048

#include "movement.h"
#include "entity.h"

typedef struct GameState_tag{
    Region *cur_region_ptr;
    EntityNode *player_avatar_node;
    EntityNode *entity_head
} GameState;

void gs_init(GameState *gs);

#endif /* GAMESTATE_H */