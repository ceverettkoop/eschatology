#ifndef REGIONTEMPLATE_H
#define REGIONTEMPLATE_H

#include "components/sprite.h"

static const int MIN = 0;
static const int MAX = 1;

typedef struct RegionTemplate_tag{
    int room_ct_range[2];
    int room_sz_range[2];
    int exit_count_opp[2];
    int exit_count_l[2];
    int exit_count_r[2];

    SpriteID default_background;
    SpriteID room_floor;
    SpriteID outdoor_barrier_a;
    SpriteID outdoor_barrier_b;
    SpriteID room_wall;
    SpriteID door;
    SpriteID border_barrier;
} RegionTemplate;

const RegionTemplate DEFAULT_REGION = {
    .room_ct_range[MIN] = 1,
    .room_ct_range[MAX] = 5,
    .room_sz_range[MIN] = 10,
    .room_sz_range[MAX] = 30,
    .exit_count_opp[]

};


#endif /* REGIONTEMPLATE_H */
