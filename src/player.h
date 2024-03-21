#ifndef PLAYER_H
#define PLAYER_H

#include "gamestate.h"

EntityID create_player(GameState *gs);
void free_player(EntityID id, GameState *gs);

#endif /* PLAYER_H */
