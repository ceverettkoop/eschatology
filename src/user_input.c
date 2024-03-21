#include "user_input.h"
#include "raylib.h"

bool is_player_action(UserInput input){
    return input;
}

bool is_ui_action(UserInput input){
    return false;
}

UserInput get_user_input(){
    int key = GetKeyPressed();
    switch (key){
    case KEY_UP:
        return MOVE_N;
        break;
    case KEY_DOWN:
        return MOVE_S;
        break;
    case KEY_LEFT:
        return MOVE_W;
        break;
    case KEY_RIGHT:
        return MOVE_E;
        break;
    case KEY_KP_9:
        return MOVE_NE;
        break;
    case KEY_KP_7:
        return MOVE_NW;
        break;
    case KEY_KP_1:
        return MOVE_SW;
        break;
    case KEY_KP_3:
        return MOVE_SE;
        break;
    case KEY_KP_5:
        return PASS_TURN;
        break;
    default:
        return 0;
        break;
    }
}