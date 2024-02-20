#include <raylib.h>
#include "graphics.h"

int main(int argc, char const *argv[]){
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Eschatology");
    SetTargetFPS(30);

    while(!WindowShouldClose()){
        draw_frame();
    }
    
    return 0;
}
