#include "raylib.h"
#include "graphics.h"

void draw_frame() {

    BeginDrawing();
        ClearBackground(WHITE);
        DrawText("Hello world\n", 100, 100, 24, BLACK);
    EndDrawing();
}