#include "regiontemplate.h"
#include "components/region.h"

const RegionTemplate DEFAULT_REGION = {
    .room_ct_range[R_MIN] = 3,
    .room_ct_range[R_MAX] = 7,
    .bsp_iterations = 4,
    .exit_count_opp[R_MIN] = 1,
    .exit_count_opp[R_MAX] = 5,
    .exit_count_l[R_MIN] = 1,
    .exit_count_l[R_MAX] = 5,
    .exit_count_r[R_MIN] = 1,
    .exit_count_r[R_MAX] = 5,
    .min_room_width = 5,
    .max_room_width = COLUMNS / 3,
    .min_room_height = 3,
    .max_room_height = ROWS / 3,
    .default_background = SPRITE_GRASS,
    .room_floor = SPRITE_STONEFLOOR,
    .outdoor_barrier_a = SPRITE_MOUNTAIN,
    .room_wall = SPRITE_WALL,
    .door = SPRITE_DOOR,
    .border_barrier = SPRITE_MOUNTAIN
};