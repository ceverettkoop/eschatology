#include "player_action.h"
#include "position.h"
#include "interaction.h"

static void handle_action(PlayerAction action, Position target);
static PlayerAction action_from_interaction(Interaction intr);

void handle_move_or_action(EntityID player_entity_id, UserInput input, GameState *gs) {
    Direction dir = user_input_to_dir(input);
    Position *player_pos = (Position*)sc_map_get_64v(&gs->Position_map, player_entity_id);
    //case input is movement
    if(dir != DIR_NONE){
        MoveResult result;
        Interaction intr_result;
        result = attempt_move(player_entity_id, dir, 1, &intr_result);
        //move results in action case, apply action to would be destination
        if(result == ACTION){
            handle_action(action_from_interaction(intr_result), calc_destination(*player_pos, dir));
        }
        //other move results are just advance frame
    //input was action not move, apply action to current pos
    }else{
        handle_action(action_from_input(input), *player_pos);
    }
}

void handle_action(PlayerAction action, Position target) {
    
}

PlayerAction action_from_interaction(Interaction intr) {
    switch (intr.interaction)
    {
    case INTR_FIGHT:
        return FIGHT;
        break;
    case INTR_TRANSPORT:
        return TRANSPORT;
        break;
    case INTR_IMPASSABLE:
        return WAIT;
        break;
    default:
        fatal_err_generic("Unreachable\n");
        break;
    }
}
