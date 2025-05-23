#include "graphics.h"

#include <stdio.h>
#include <string.h>

#include "components/position.h"
#include "components/sprite.h"
#include "gamestate.h"
#include "raylib.h"
#include "raymath.h"
#include "vector.h"

#define MAX_PATH_LEN 1024
#define MAX_FILENAME_LEN 64

static const Vector2 REGION_ORIGIN = (Vector2){6, 4};
static const int INSET_RANGE_FROM_CENTER = 3;
static const int INSET_TILE_RANGE = (INSET_RANGE_FROM_CENTER * 2) + 1;
static const Vector2 INSET_ORIGIN = (Vector2){6, 250};

static Image small_sprite_pngs[SPRITE_COUNT];
static Texture2D small_sprites[SPRITE_COUNT];

static Image large_sprite_pngs[SPRITE_COUNT];
static Texture2D large_sprites[SPRITE_COUNT];

static void draw_region_map(GameState *gs);
static void draw_inset_map(GameState *gs, Position loc);
static void load_small_sprite_texture(SpriteID id);
static void load_large_sprite_texture(SpriteID id);

static Vector sprite_update_queue;

RenderTexture2D main_render_texture;

void init_graphics() {
    main_render_texture = LoadRenderTexture(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR);
    for (SpriteID i = 0; i < SPRITES_IMPLEMENTED; i++) {
        load_small_sprite_texture(i);
    }
    for (SpriteID i = 0; i < SPRITES_IMPLEMENTED; i++) {
        load_large_sprite_texture(i);
    }
    sprite_update_queue = new_vector(sizeof(Position));
};

//updates must occur prior, this is just drawing what is loaded into the region
void draw_frame(GameState *gs, Position player_loc) {
    Rectangle source = {0, (float)-SCREEN_HEIGHT, (float)SCREEN_WIDTH, (float)-SCREEN_HEIGHT};
    Rectangle dest = {0, 0, (float)SCREEN_WIDTH * SCALE_FACTOR, (float)SCREEN_HEIGHT * SCALE_FACTOR};

    BeginTextureMode(main_render_texture);
    ClearBackground(WHITE);
    draw_region_map(gs);
    // draw UI here
    draw_inset_map(gs, player_loc);
    EndTextureMode();

    BeginDrawing();
    DrawTexturePro(main_render_texture.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
}

static void draw_region_map(GameState *gs) {
    Region *reg_ptr = gs->cur_region_ptr;
    for (int i = 0; i < ROWS; i++) {
        for (int n = 0; n < COLUMNS; n++) {
            Vector2 offset = (Vector2){n * SMALL_SPRITE_WIDTH, i * SMALL_SPRITE_HEIGHT};
            Vector2 origin = Vector2Add(offset, REGION_ORIGIN);
            Position pos = {.row = i, .column = n, .region_ptr = reg_ptr};
            SpriteID id = reg_ptr->displayed_sprite[i][n];
            Texture2D sprite = small_sprites[id];
            DrawTexture(sprite, (int)origin.x, (int)origin.y, WHITE);
        }
    }
}

static void draw_inset_map(GameState *gs, Position loc) {
    Region *reg_ptr = gs->cur_region_ptr;
    // out of bounds values expected here:
    int pos_row_offset = loc.row - INSET_RANGE_FROM_CENTER;
    int pos_column_offset = loc.column - INSET_RANGE_FROM_CENTER;

    for (int i = 0; i < INSET_TILE_RANGE; i++) {
        for (int n = 0; n < INSET_TILE_RANGE; n++) {
            Vector2 screen_offset = (Vector2){n * LARGE_SPRITE_WIDTH, i * LARGE_SPRITE_HEIGHT};
            Vector2 origin = Vector2Add(screen_offset, INSET_ORIGIN);
            // pos in region iterates like normal from starting point
            Position pos = {.row = i + pos_row_offset, .column = n + pos_column_offset, .region_ptr = reg_ptr};
            // out of range positions rendered blank
            SpriteID id = pos_is_valid(pos) ? reg_ptr->displayed_sprite[pos.row][pos.column] : SPRITE_BLANK.id;
            Texture2D sprite = large_sprites[id];
            DrawTexture(sprite, (int)origin.x, (int)origin.y, WHITE);
        }
    }
}

static void load_small_sprite_texture(SpriteID id) {
    char path[MAX_PATH_LEN];
    snprintf(path, MAX_PATH_LEN - 1, "../resources/sprites/small/%d.png", id);
    small_sprite_pngs[id] = LoadImage(path);
    small_sprites[id] = LoadTextureFromImage(small_sprite_pngs[id]);
    UnloadImage(small_sprite_pngs[id]);
}

static void load_large_sprite_texture(SpriteID id) {
    char path[MAX_PATH_LEN];
    snprintf(path, MAX_PATH_LEN - 1, "../resources/sprites/large/%d.png", id);
    large_sprite_pngs[id] = LoadImage(path);
    large_sprites[id] = LoadTextureFromImage(large_sprite_pngs[id]);
    UnloadImage(large_sprite_pngs[id]);
}

void queue_sprite_update(Position pos){
    vec_push_back(&sprite_update_queue, &pos, 1);
}