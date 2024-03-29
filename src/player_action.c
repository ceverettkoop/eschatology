#include "player_action.h"
#include "position.h"

static void attempt_player_move(Position *origin, Direction dir, int distance);

void handle_player_action(EntityID player_entity_id, UserInput input, GameState *gs) {
    Direction dir = user_input_to_dir(input);
    if(dir != DIR_NONE){
        Position *player_pos = (Position*)sc_map_get_64v(&gs->Position_map, player_entity_id);
        attempt_move(player_entity_id, dir, 1);
    }
}