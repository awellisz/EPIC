#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"

// struct for a move, keeps track of move and move's score
typedef struct {
    int move;
    int score;
} move_t;

// struct for a move list -- array of moves and number of moves on the move list
typedef struct {
    move_t moves[MAX_POSITION_MOVES];
    int count;
} movelist_t;

void generate_all_moves(const board_t *pos, movelist_t *list);
bool move_exists(board_t *pos, const int move);

#endif