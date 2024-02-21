#include "raylib.h"
#include "graphics.h"
#include "user_input.h"
#include "region.h"
#include <stdlib.h>

int main(int argc, char const *argv[]){

    //init game_state
    Region *cur_region_ptr = generate_region();
    generate_neighbors(cur_region_ptr);

    InitWindow(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, "eschatology");
    SetTargetFPS(30);
    init_graphics();
    
    //main loop
    while(!WindowShouldClose()){

        //update only on input
        UserInput input = get_user_input();
        //determine if input is an action or UI related
        if(is_player_action(input)){
        }
        //draw
        draw_frame(cur_region_ptr);
    }
    
    return 0;
}
