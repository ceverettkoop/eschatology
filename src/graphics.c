#include "raylib.h"
#include "raymath.h"
#include "graphics.h"
#include "tile.h"
#include "string.h"
#include "stdio.h"

#define MAX_PATH_LEN 1024
#define MAX_FILENAME_LEN 64

static const Vector2 REGION_ORIGIN = (Vector2){6,4}; 

static Image small_sprite_pngs[SPRITE_COUNT];
static Texture2D small_sprites[SPRITE_COUNT];

static void draw_region_map(const Region *reg_ptr);
static void load_small_sprite_texture(SpriteID id);

void init_graphics(){
    //just for testing
    load_small_sprite_texture(SPRITE_MOUNTAIN);
    load_small_sprite_texture(SPRITE_GRASS);
};

void draw_frame(const Region *reg_ptr) {

    RenderTexture2D render_texture = LoadRenderTexture(SCREEN_WIDTH * SCALE_FACTOR, 
        SCREEN_HEIGHT * SCALE_FACTOR);
    Rectangle source = {0, (float) -SCREEN_HEIGHT, (float) SCREEN_WIDTH, (float) -SCREEN_HEIGHT};
    Rectangle dest = {0, 0, (float) SCREEN_WIDTH * SCALE_FACTOR, (float) SCREEN_HEIGHT * SCALE_FACTOR};

    BeginTextureMode(render_texture);
        ClearBackground(WHITE);
        draw_region_map(reg_ptr);
    EndTextureMode(); 

    BeginDrawing();
        DrawTexturePro(render_texture.texture, source, dest, (Vector2){0,0}, 0.0f, WHITE);
    EndDrawing();
}

static void draw_region_map(const Region *reg_ptr){
    for (int i = 0; i < ROWS; i++){
        for (int n = 0; n < COLUMNS; n++){
            const Tile *tile = &(reg_ptr->tile_matrix[i][n]);
            Vector2 offset = (Vector2){n * SMALL_SPRITE_WIDTH, i * SMALL_SPRITE_HEIGHT };
            Vector2 origin = Vector2Add(offset, REGION_ORIGIN);
            Texture2D sprite = small_sprites[tile->properties_ptr->background];
            DrawTexture(sprite, origin.x, origin.y, WHITE);
        }
    }
}

static void load_small_sprite_texture(SpriteID id){
    char path[MAX_PATH_LEN];
    snprintf(path, MAX_PATH_LEN - 1 , "../resources/sprites/small/%d.png", id);
    small_sprite_pngs[id] = LoadImage(path);
    small_sprites[id] = LoadTextureFromImage(small_sprite_pngs[id]);
    UnloadImage(small_sprite_pngs[id]);
}