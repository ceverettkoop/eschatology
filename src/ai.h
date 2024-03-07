#ifndef AI_H
#define AI_H

#include "movement.h"
#include "entity.h"
#include "action.h"

typedef struct AIAction_tag{
    bool move_next_frame;
    Direction dir;
    bool act_next_frame;
    Action act;
    void *action_target; //eg another entity
    void *action_object; //another entity, inventory item, spell etc
}AIAction;

AIAction get_next_action(Entity *actor);


#endif /* AI_H */
