#ifndef SPRITES_H
#define SPRITES_H

#include "stdint.h"

#define SPRITE_GRASS 0
#define SPRITE_MOUNTAIN 1
#define SPRITE_PLAYER 2

#define SPRITE_DOOR 3
#define SPRITE_WALL 4
#define SPRITE_STONEFLOOR 5
#define SPRITE_COUNT 6
#define SMALL_SPRITES_IMPLEMENTED 3

#define SMALL_SPRITE_WIDTH 6
#define SMALL_SPRITE_HEIGHT 12

//corresponds to a small sprite and a big sprite
typedef uint8_t SpriteID;

#endif /* SPRITES_H */
