#include "movement.h"

TilePos move(TilePos origin, Direction dir, int distance){

    TilePos ret_val;
    switch (dir){
    case DIR_N:
        ret_val.column = origin.column;
        ret_val.row = origin.row - distance;
        break;
    case DIR_NE:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row - 1;
        break;
    case DIR_E:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row;
        break;
    case DIR_SE:
        ret_val.column = origin.column + distance;
        ret_val.row = origin.row + distance;
        break;
    case DIR_S:
        ret_val.column = origin.column;
        ret_val.row = origin.row + distance;
        break;
    case DIR_SW:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row + distance;
        break;
    case DIR_W:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row;
        break;
    case DIR_NW:
        ret_val.column = origin.column - distance;
        ret_val.row = origin.row - distance;
        break;
    default:
        break;
    }

    //overflow
    if(ret_val.row >= ROWS ){
        ret_val.row = ROWS - 1;
    }
    if(ret_val.row < 0 ){
        ret_val.row = 0;
    }
    if(ret_val.column >= COLUMNS ){
        ret_val.column = COLUMNS - 1;
    }
    if(ret_val.column < 0 ){
        ret_val.column = 0;
    }

}