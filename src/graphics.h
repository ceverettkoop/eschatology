#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "region.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCALE_FACTOR 2

void init_graphics();
void draw_frame(const Region *reg_ptr);

#endif /* GRAPHICS_H */
