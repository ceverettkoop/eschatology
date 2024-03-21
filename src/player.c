#include "player.h"
#include "gamestate.h"
#include "components/region.h"
#include "components/name.h"
#include "components/position.h"
#include "components/sprite.h"

#define PLAYER_ORIGIN_POS (Position){REGION_HEIGHT / 2, REGION_HEIGHT / 2 , gs->cur_region_ptr}

EntityID create_player(GameState *gs){
    EntityID name_id = new_entity(gs);
    add_Name(name_id, (Name){"Player"}, gs);
    add_Position(name_id, PLAYER_ORIGIN_POS, gs);
    add_Sprite(name_id, SPRITE_PLAYER, gs);
}

void free_player(EntityID id, GameState *gs){
    free_Name(id, gs);
    free_Position(id, gs);
    free_Sprite(id, gs);
    free_entity(id, gs);
}