#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>

typedef uint64_t EntityID;

#define NEW_ENTITY_FUNC_NAME(type) init_##type
#define NEW_ENTITY_DECL(gs, ...) 
    EntityID NEW_ENTITY_FUNC_NAME(GameState* gs, )


#define NEW_ENTITY_DEF(gs, ...) /
    EntityID new_id = NEW_ENTITY_FUNC_NAME(gs);




#endif /* ENTITY_H */
