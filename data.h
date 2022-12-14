#ifndef DATA_H 
#define DATA_H

#include <stdbool.h>

// for print board 
extern char pce_char[];
extern char side_char[];
extern char rank_char[];
extern char file_char[];
// characteristics of pieces 
extern bool piece_big[13];
extern bool piece_maj[13];
extern bool piece_min[13];
extern int piece_val[13];
extern int piece_col[13];
extern bool piece_knight[13];
extern bool piece_king[13];
extern bool piece_rook_queen[13];
extern bool piece_bishop_queen[13];
extern bool piece_slides[13];
extern bool piece_pawn[13];
//  for movegen.c
extern int pce_dir[13][8];

// For attack.c: check piece type
#define IsBQ(p) (piece_bishop_queen[(p)])
#define IsRQ(p) (piece_rook_queen[(p)])
#define IsN(p) (piece_knight[(p)])
#define IsK(p) (piece_king[(p)])

#endif 