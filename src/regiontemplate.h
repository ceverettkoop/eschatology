#ifndef REGIONTEMPLATE_H
#define REGIONTEMPLATE_H

#include "components/sprite.h"

#define R_MIN 0
#define R_MAX 1

typedef struct RegionTemplate_tag{
    int room_ct_range[2];
    int bsp_iterations;
    int exit_count_opp[2];
    int exit_count_l[2];
    int exit_count_r[2];
    int min_room_width;
    int max_room_width;
    int min_room_height;
    int max_room_height;

    Sprite default_background;
    Sprite room_floor;
    Sprite outdoor_barrier_a;
    Sprite outdoor_barrier_b;
    Sprite room_wall;
    Sprite door;
    Sprite border_barrier;
} RegionTemplate;

extern const RegionTemplate DEFAULT_REGION;

#endif /* REGIONTEMPLATE_H */
