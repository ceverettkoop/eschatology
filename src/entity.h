#ifndef ENTITY_H
#define ENTITY_H


#include "entitytypes.h"
#include "stdbool.h"
#include "region.h"

//everything rendered on map other than background tiles
typedef struct Entity_tag{
    bool is_mobile;
    SpriteID sprite;
    Region *region_ptr;
    TilePos pos;
    void* properties_ptr;
}Entity;

typedef struct EntityNode_tag{
    Entity *ptr;
    EntityNode *prev;
    EntityNode *next;
}EntityNode;

//entity that can be interacted with but does not move
typedef struct StaticProperties_tag{
    bool can_be_taken;
    bool is_passable;
    int quantity;
}StaticProperties;

typedef struct MobileProperties_tag{
    bool is_player;
    bool is_passable;
}MobileProperties;

EntityNode *new_entity(EntityNode *list_head, SpriteID _sprite,
    Region *_region_ptr, TilePos _pos, EntityType type);
void free_entity(EntityNode *ptr);

#endif /* ENTITY_H */
