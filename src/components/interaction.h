#ifndef INTERACTION_H
#define INTERACTION_H

#include "../gamestate.h"
#include "component.h"
#include "sc_map.h"

// if something that is not a tile will interact with the player when it is moved onto their tile, this component
// indicates that

typedef enum {
    INTR_FIGHT,
    INTR_TRANSPORT,
    INTR_IMPASSABLE
} InteractionResult;

typedef struct Interaction_tag {
    InteractionResult interaction;
    void *dest_region;
    int dest_row;
    int dest_column;
} Interaction;

ADD_COMPONENT_DECL(Interaction);
FREE_COMPONENT_DECL(Interaction);

#endif /* INTERACTION_H */
