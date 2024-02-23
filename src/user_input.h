#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <stdbool.h>

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
} UserInput;

bool is_player_action(UserInput input);
bool is_ui_action(UserInput input);
UserInput get_user_input();

#endif /* USER_INPUT_H */
