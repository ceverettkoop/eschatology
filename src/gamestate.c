#include "gamestate.h"
#include "avatar.h"
#include <stdlib.h>

void gs_init(GameState *gs){
    gs->entity_head = NULL;
    gs->cur_region_ptr = generate_region();
    generate_neighbors(gs->cur_region_ptr);

}

void update_gamestate(GameState *gs){


}