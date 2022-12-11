// validate.c
// 11/17/2022

#include <stdbool.h>

#include "defs.h"
#include "init.h"
#include "validate.h"

// Five functions to make sure the arguments to other functions are correct
// Hopefully will prevent improperly indexing arrays etc.

bool sq_on_board(const int sq) {
    return files_brd[sq] != OFFBOARD;
}

bool side_valid(const int side) {
    return (side == WHITE) || (side == BLACK);
}

bool file_rank_valid(const int fr) {
    return (fr >=0 && fr <= 7);
}

bool piece_valid_empty(const int pce) {
    return (pce >= EMPTY && pce <= bK);
}

bool piece_valid(const int pce) { 
    return (pce >= wP && pce <= bK);
}