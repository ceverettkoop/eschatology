#include "player.h"
#include "gamestate.h"
#include "components/name.h"
#include "components/position.h"
#include "components/sprite.h"


void init_player(EntityID id, GameState *gs){
    EntityID name_id = new_entity(gs);
    add_Name(name_id, "Player", gs);


}