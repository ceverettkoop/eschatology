#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H

#include <stdbool.h>

typedef enum{
    PLAYER,
    ORC
} EntityType;

void *prop_init_pointer(EntityType type, bool *mobile);

#endif /* ENTITYTYPES_H */
