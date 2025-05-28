#include <stdlib.h>

#include "components/region.h"
#include "error.h"
#include "gamestate.h"
#include "graphics.h"
#include "raylib.h"
#include "user_input.h"
#include "player_action.h"

int main(int argc, char const *argv[]) {
    EntityID player_id;
    // init game_state
    GameState *gs = malloc(sizeof(GameState));
    check_malloc(gs);
    gs_init(gs);
    player_id = get_player_id();

    InitWindow(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, "eschatology");
    SetTargetFPS(30);
    init_graphics();

    // main loop
    while (!WindowShouldClose()) {
        // update only on input
        UserInput input = get_user_input();
        // determine if input is an action or UI related
        if (is_player_action(input)) {
            handle_move_or_action(player_id, input, gs);
            update_all_sprites(gs->cur_region_ptr);
        } else if (is_ui_action(input)) {
            // update_ui, make ui.h
        }
        // draw
        Position *cur_pos = sc_map_get_64v(&gs->Position_map, player_id);
        if(!cur_pos) err_entity_not_found();
        draw_frame(gs, *cur_pos);
    }

    return 0;
}
