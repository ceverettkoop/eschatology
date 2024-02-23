#include "raylib.h"
#include "graphics.h"
#include "user_input.h"
#include "region.h"
#include "gamestate.h"
#include "error.h"
#include <stdlib.h>

int main(int argc, char const *argv[]){

    //init game_state
    GameState *gs = malloc(sizeof(GameState));
    check_malloc(gs);
    gs_init(gs);

    InitWindow(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, "eschatology");
    SetTargetFPS(30);
    init_graphics();
    
    //main loop
    while(!WindowShouldClose()){

        //update only on input
        UserInput input = get_user_input();
        //determine if input is an action or UI related
        if(is_player_action(input)){
            update_gamestate(gs);
            
        }else if(is_ui_action(input)){
            //update_ui
        }

        //draw
        draw_frame(gs->cur_region_ptr);    
    }
    
    return 0;
}
