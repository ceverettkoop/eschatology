#ifndef ENTITY_H
#define ENTITY_H

#include "stdbool.h"

//everything rendered on map
//trees, mountains, etc are not unique and not passable/have no properties
//grass, desert is passable but not unique
typedef struct Entity_tag{
    bool is_mobile;
    void* properties_ptr;
}Entity;

//entity that can be interacted with but does not move
typedef struct StaticProperties_tag{
    bool can_be_taken;
    int quantity;

}StaticProperties;

typedef struct MobileProperties_tag{
    bool is_player;


}MobileProperties;


#endif /* ENTITY_H */
