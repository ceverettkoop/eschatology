#ifndef PLAYER_ACTION_H
#define PLAYER_ACTION_H

#include "gamestate.h"
#include "user_input.h"

typedef enum{
    FIGHT,
    PICKUP,
    TALK,
    TRANSPORT,
    WAIT
}PlayerAction;

void handle_move_or_action(EntityID player_entity_id, UserInput input, GameState *gs);

#endif /* PLAYER_ACTION_H */
