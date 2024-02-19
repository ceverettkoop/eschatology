#include <raylib.h>


int main(int argc, char const *argv[]){
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Eschatology");
    SetTargetFPS(30);

    while(!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(WHITE);
            DrawText("Hello world\n", 100, 100, 24, BLACK);
        EndDrawing();
    }
    
    return 0;
}
