#ifndef COMPONENT_H
#define COMPONENT_H

#include "../entityid.h"
#include "../error.h"

#define TYPE_MAP(type) type##_map

#define ADD_FUNC_NAME(type) add_##type
#define ADD_COMPONENT_DECL(type) void ADD_FUNC_NAME(type)(EntityID id, type value, GameState * gs);
#define ADD_COMPONENT_FUNC(type)                                        \
    void ADD_FUNC_NAME(type)(EntityID id, type value, GameState * gs) { \
        void *ptr = malloc(sizeof(type));                               \
        check_malloc(ptr);                                              \
        void *old_val = sc_map_put_64v(&gs->TYPE_MAP(type), id, ptr);    \
        if (sc_map_found(&gs->TYPE_MAP(type))) {                         \
            free(old_val);                                              \
            err_component_exists(#type);                                \
        }                                                               \
        *((type *)ptr) = value;                                         \
    }

#define FREE_FUNC_NAME(type) free_##type
#define FREE_COMPONENT_DECL(type) void FREE_FUNC_NAME(type)(EntityID id, GameState * gs);
#define FREE_COMPONENT_FUNC(type)                                         \
    void FREE_FUNC_NAME(type)(EntityID id, GameState * gs) {  \
        void *component_to_free = sc_map_get_64v(&gs->TYPE_MAP(type), id); \
        if (sc_map_found(&gs->TYPE_MAP(type))) {                           \
            free(component_to_free);                                      \
            sc_map_del_64v(&gs->TYPE_MAP(type), id);                       \
        } else {                                                          \
            err_free_missing_component(#type);                            \
        }                                                                 \
    }

#endif /* COMPONENT_H */
