// data.c

#include <stdbool.h>

#include "defs.h"

char pce_char[] = ".PNBRQKpnbrqk";
char side_char[] = "wb-";
char rank_char[] = "12345678";
char file_char[] = "abcdefgh";

// RECALL: PIECE ENUM IS 
// EMPTY, 
// wP, wN, wB, wR, wQ, wK, 
// bP, bN, bB, bR, bQ, bK

bool piece_big[13] = { 
    false, 
    false, true, true, true, true, true, 
    false, true, true, true, true, true,
};
bool piece_maj[13] = {
    false,
    false, false, false, true, true, true, 
    false, false, false, true, true, true, 
};
bool piece_min[13] = {
    false,
    false, true, true, false, false, false, 
    false, true, true, false, false, false, 
};
int piece_val[13] = {
    0,
    100, 325, 325, 550, 1000, 50000,
    100, 325, 325, 550, 1000, 50000,
};
int piece_col[13] = {
    BOTH,
    WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, 
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, 
};

// Is the piece we just hit in attack.c a knight? (etc.)
bool piece_knight[13] = {
    false,
    false, true, false, false, false, false, 
    false, true, false, false, false, false
};
bool piece_king[13] = {
    false,
    false, false, false, false, false, true, 
    false, false, false, false, false, true
};
bool piece_rook_queen[13] = {
    false,
    false, false, false, true, true, false, 
    false, false, false, true, true, false
};
bool piece_bishop_queen[13] = {
    false,
    false, false, true, false, true, false,
    false, false, true, false, true, false
};
bool piece_slides[13] = {
    false,
    false, false, true, true, true, false,
    false, false, true, true, true, false,
};

int pce_dir[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 12, 21, 19, 8},
    {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},
    {-1 } // FINISH THIS
};
 
// RECALL: PIECE ENUM IS 
// EMPTY, 
// wP, wN, wB, wR, wQ, wK, 
// bP, bN, bB, bR, bQ, bK