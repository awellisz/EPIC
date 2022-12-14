// evaluate.c

#include <assert.h>
#include <stdio.h>

#include "board.h"
#include "defs.h"
#include "evaluate.h"
#include "init.h"
#include "validate.h"

// Depending on where a pawn is on the board, give it the corresponding value
// E.g. if white has a pawn on D2, value is minus 10
// if pawn moves from D2 to D4: net gain of 30 centipawns
// NB: A1 is the top right corner
const int pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     10,  10,   0, -10, -10,   0,  10,  10,
      5,   0,   0,   5,   5,   0,   0,   5,
      0,   0,  10,  20,  20,  10,   0,   0,
      5,   5,   5,  10,  10,   5,   5,   5,
     10,  10,  10,  20,  20,  10,  10,  10, 
     20,  20,  20,  30,  30,  20,  20,  20, 
      0,   0,   0,   0,   0,   0,   0,   0
};

// Values for the below taken from https://www.chessprogramming.org/Simplified_Evaluation_Function

// Knights should avoid the corners/edges
const int knight_table[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};

// Avoid corners and borders
const int bishop_table[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};

// Centralize, occupy the 7th rank, avoid a/h files
const int rook_table[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

// Pretty mild discouragement from edges/corners
const int queen_table[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// Mirror index so we can use white's tables (above) for black
const int mirror_64[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
     8,  9, 10, 11, 12, 13, 14, 15, 
     0,  1,  2,  3,  4,  5,  6,  7
};

// Return evaluation of a position, in hundredths of a pawn
int evaluate_position(board_t *pos) {
    int piece; 
    int piece_num;
    int sq;
    // Default: material score for white minus material score for black
    // White's material is positive; black's is negative
    int score = pos->material[WHITE] - pos->material[BLACK];

    piece = wP;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score += pawn_table[SQ64(sq)];
    }

    piece = bP; 
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score -= pawn_table[mirror_64[SQ64(sq)]];
    }
    
    piece = wN;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score += knight_table[SQ64(sq)];
    }

    piece = bN;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score -= knight_table[mirror_64[SQ64(sq)]];
    }

    piece = wR;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score += rook_table[SQ64(sq)];
    }

    piece = bR;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score -= rook_table[mirror_64[SQ64(sq)]];
    }

    piece = wQ;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score += queen_table[SQ64(sq)];
    }

    piece = bQ;
    for (piece_num = 0; piece_num < pos->pce_num[piece]; piece_num++) {
        sq = pos->pce_list[piece][piece_num];
        assert(sq_on_board(sq));
        score -= queen_table[mirror_64[SQ64(sq)]];
    }
    return 0;
}