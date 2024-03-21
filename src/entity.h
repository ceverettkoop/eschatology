#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>

typedef uint64_t EntityID;

// evil macro trick
// see https://codecraft.co/2014/11/25/variadic-macros-tricks/
#define _GET_NTH_ARG(_1, _2, _3, _4, N, ...) N
// Count how many args are in a variadic macro. Only works for up to N-1 args.
#define COUNT_VARARGS(...) _GET_NTH_ARG(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define MAX_COMPONENT_PER_ENTITY 11

#define NEW_ENTITY_FUNC_NAME(type) init_##type

#define NEW_ENTITY_DECL(type, ...) EntityID NEW_ENTITY_FUNC_NAME(type)(GameState* gs, __VA_ARGS__);

#define NEW_ENTITY_DEF(type, ...)   \
    EntityID NEW_ENTITY_FUNC_NAME(type)(GameState* gs, __VA_ARGS__){    \
        int arg_count = COUNT_VARARGS(__VA_ARGS__); \
    }


#endif /* ENTITY_H */
