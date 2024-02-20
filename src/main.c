#include "raylib.h"
#include "graphics.h"
#include "user_input.h"
#include "region.h"
#include <stdlib.h>

int main(int argc, char const *argv[]){
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    //init game_state

    Region *cur_region_ptr = generate_region();

    //init graphics
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Eschatology");
    SetTargetFPS(30);
    
    //main loop
    while(!WindowShouldClose()){

        //update only on input
        UserInput input = get_user_input();
        //determine if input is an action or UI related
        if(is_player_action(input)){
            
        }

        //draw
        draw_frame();
    }
    
    return 0;
}
