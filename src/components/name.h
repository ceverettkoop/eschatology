#ifndef NAME_H
#define NAME_H

#include "component.h"
#include "../gamestate.h"
#include "sc_map.h"

#define NAME_MAX 128

typedef struct Name_tag{
    char name[NAME_MAX];
}Name;

ADD_COMPONENT_DECL(Name);
FREE_COMPONENT_DECL(Name);

#endif /* NAME_H */
