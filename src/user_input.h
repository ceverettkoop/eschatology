#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <stdbool.h>
#include "position.h"

typedef enum{
    NO_INPUT = 0,
    MOVE_N,
    MOVE_NE,
    MOVE_E,
    MOVE_SE,
    MOVE_S,
    MOVE_SW,
    MOVE_W,
    MOVE_NW,
    PASS_TURN
} UserInput;

bool is_player_action(UserInput input);
bool is_ui_action(UserInput input);
UserInput get_user_input();
Direction user_input_to_dir(UserInput user_in);

#endif /* USER_INPUT_H */
