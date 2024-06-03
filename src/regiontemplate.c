#include "regiontemplate.h"

const RegionTemplate DEFAULT_REGION = {
    .room_ct_range[R_MIN] = 1,
    .room_ct_range[R_MAX] = 5,
    .bsp_iterations = 4,
    .exit_count_opp[R_MIN] = 1,
    .exit_count_opp[R_MAX] = 5,
    .exit_count_l[R_MIN] = 1,
    .exit_count_l[R_MAX] = 5,
    .exit_count_r[R_MIN] = 1,
    .exit_count_r[R_MAX] = 5,
    .default_background = SPRITE_GRASS,
    .room_floor = SPRITE_STONEFLOOR,
    .outdoor_barrier_a = SPRITE_MOUNTAIN,
    .room_wall = SPRITE_WALL,
    .door = SPRITE_DOOR,
    .border_barrier = SPRITE_MOUNTAIN
};