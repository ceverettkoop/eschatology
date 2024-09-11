#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "components/region.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCALE_FACTOR 2

void init_graphics();
void draw_frame(GameState *gs, Position player_loc);
void queue_sprite_update(Position pos);

#endif /* GRAPHICS_H */
