#ifndef ENTITY_H
#define ENTITY_H

#include "stdbool.h"
#include "region.h"
#include "entitytypes.h"
#include "attributes.h"
#include "inventory.h"
#include "ai.h"

//everything rendered on map other than background tiles
typedef struct Entity_tag{
    SpriteID sprite;
    Region *region_ptr;
    TilePos pos;
    EntityType type; //to know what to cast prop_ptr to
    void* prop_ptr; //unique to this entity, generated on spawn
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
    bool is_destroyable;
    ItemType type;
    int hit_points;
    int quantity;
}StaticProperties;

typedef struct MobileProperties_tag{
    Attributes attrib;
    InventoryNode *inventory_head;
    AIProfile ai_type;
}MobileProperties;

typedef struct PlayerProperties_tag{
    Attributes attrib;
    InventoryNode *inventory_head;
}PlayerProperties;


EntityNode *new_entity(EntityNode *list_head, SpriteID _sprite,
    Region *_region_ptr, TilePos _pos, EntityType type);
void free_entity(EntityNode *ptr);

#endif /* ENTITY_H */
