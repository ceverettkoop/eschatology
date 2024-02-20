#ifndef ENTITY_H
#define ENTITY_H

#include "stdbool.h"

typedef int EntityType;
enum{
    IMPASSABLE_TERRAIN,
    PASSABLE_TERRAIN,
    STATIC_ENTITY,
    MOBILE_ENTITY
};

//everything rendered on map
//trees, mountains, etc are not unique and not passable/have no properties
//grass, desert is passable but not unique
typedef struct Entity_tag{
    EntityType type;
    void* properties_ptr;
}Entity;

typedef struct ImpassableProperties_tag{
    bool is_door;
}ImpassableProperties;

//terrain that can be passed
typedef struct PassableProperties_tag{
    int damage_per_turn;
}PassableProperties;

//entity that can be interacted with but does not move
typedef struct StaticProperties_tag{
    bool can_be_taken;
    int quantity;

}StaticProperties;

typedef struct MobileProperties_tag{
    bool is_player;


}MobileProperties;


#endif /* ENTITY_H */
