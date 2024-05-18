#include "user_input.h"
#include "raylib.h"
#include "error.h"

bool is_player_action(UserInput input){
    // placeholder
    return true;
}

bool is_ui_action(UserInput input){
    // placeholder
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
        return NO_INPUT;
        break;
    }
}

Direction user_input_to_dir(UserInput user_in) { 
    switch (user_in){
    case MOVE_N:
        return DIR_N;
        break;
    case MOVE_NE:
        return DIR_NE;
        break;
    case MOVE_E:
        return DIR_E;
        break;
    case MOVE_SE:
        return DIR_SE;
        break;
    case MOVE_S:
        return DIR_S;
        break;
    case MOVE_SW:
        return DIR_SW;
        break;
    case MOVE_W:
        return DIR_W;
        break;      
    case MOVE_NW:
        return DIR_NW;
        break;                                              
    default:
        return DIR_NONE;
        break;
    }
    
}
