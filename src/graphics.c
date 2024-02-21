#include "raylib.h"
#include "raymath.h"
#include "graphics.h"
#include "tile.h"
#include "string.h"
#include "stdio.h"

#define MAX_PATH_LEN 1024
#define MAX_FILENAME_LEN 64

static const Vector2 REGION_ORIGIN = (Vector2){10,10}; 

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
    BeginDrawing();
        ClearBackground(WHITE);
        draw_region_map(reg_ptr);
    EndDrawing();
}

static void draw_region_map(const Region *reg_ptr){
    for (int i = 0; i < REGION_WIDTH; i++){
        for (int n = 0; n < REGION_HEIGHT; n++){
            Vector2 offset = (Vector2){i * SMALL_SPRITE_WIDTH, n * SMALL_SPRITE_HEIGHT };
            Vector2 origin = Vector2Add(offset, REGION_ORIGIN);
            const Tile *tile = &(reg_ptr->tile_matrix[i][n]); 
            Texture2D sprite = small_sprites[tile->properties_ptr->background];
            DrawTexture(sprite, origin.x, origin.y, WHITE);
        }
    }
}

static void load_small_sprite_texture(SpriteID id){
    char path[MAX_PATH_LEN];
    char filename[MAX_FILENAME_LEN];
    memcpy(path, "../resources/sprites/small/", MAX_PATH_LEN);
    snprintf(filename, MAX_FILENAME_LEN - 1 , "%d.png", id);
    strncat(path, filename, MAX_PATH_LEN - MAX_FILENAME_LEN - 1);
    small_sprite_pngs[id] = LoadImage(path);
    small_sprites[id] = LoadTextureFromImage(small_sprite_pngs[id]);
    UnloadImage(small_sprite_pngs[id]);
}