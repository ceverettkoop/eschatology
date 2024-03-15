#ifndef SPRITE_H
#define SPRITE_H

#include "stdint.h"
#include "component.h"

#define SPRITE_GRASS (Sprite){0,0}
#define SPRITE_MOUNTAIN (Sprite){1,0}
#define SPRITE_PLAYER (Sprite){2,9}

#define SPRITE_DOOR (Sprite){3,1}
#define SPRITE_WALL (Sprite){4,1}
#define SPRITE_STONEFLOOR (Sprite){5,1}
#define SPRITE_COUNT 6
#define SMALL_SPRITES_IMPLEMENTED 3

#define SMALL_SPRITE_WIDTH 6
#define SMALL_SPRITE_HEIGHT 12

//corresponds to a small sprite and a big sprite
typedef uint32_t SpriteID;
typedef int8_t DrawPriority;

typedef struct Sprite_tag{
    SpriteID id;
    DrawPriority draw_priority;
}Sprite;

ADD_COMPONENT_DECL(Sprite);
FREE_COMPONENT_DECL(Sprite);

#endif /* SPRITE_H */
