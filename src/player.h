#ifndef PLAYER_H
#define PLAYER_H

#include "gamestate.h"

void create_player(EntityID id, GameState *gs);
void free_player(EntityID id, GameState *gs);

#endif /* PLAYER_H */
