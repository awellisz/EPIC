#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
// to disable assertions:
// #define NDEBUG 
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

//typedef unsigned long long U64;
typedef uint64_t U64;

#define NAME "EPIC" // E.P.I.C. Plays Incredible Chess
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INF 1000000

enum { MAX_DEPTH = 64 };
enum { BRD_SQ_NUM = 120 };
enum { MAX_GAME_MOVES = 2048 };
enum { MAX_POSITION_MOVES = 256 };
// maximum number of possible moves in a position -- seems to actually be 218

enum { NOMOVE = 0 };

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };

enum {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFFBOARD
};

// White/black king/queen castling permissions
// 1, 2, 4, 8 so that perms can be represented by just 4 bits total
// e.g. 1 0 0 1 (0x9) indicates WKCA and BQCA
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };


/************************
 * GAME MOVE REPRESENTATION
 ************************/

/*
- In the 120-sq representation, the lowest square is 21 and the higest is 98.
98 requires 7 bits to represent in binary. So, we can encode the "from" square
and the "to" square in each move as 7 bits of a large integer. The 1 bits below 
show which bits of the larger integer are used for each category.
- We need to keep track of the piece that's captured (requires up to 4 bits)
- We need to keep track of whether it was an En Passant capture (1 bit)
- Whether a move was a pawn start or not (1 bit)
- if we promoted, what piece did we promote to? (4 bits)
    - perhaps could implement with 3 bits
    - e.g. 000=N/A, 001 = B, 010=N, 011=R, 100=Q ?
- whether a move was a castling move or not (1 bit)

0000 0000 0000 0000 0000 0111 1111 <- From square
0000 0000 0000 0011 1111 1000 0000 <- To square
0000 0000 0011 1100 0000 0000 0000 <- captured piece
0000 0000 0100 0000 0000 0000 0000 <- en passant?
0000 0000 1000 0000 0000 0000 0000 <- pawn start?
0000 1111 0000 0000 0000 0000 0000 <- promotion?
0001 0000 0000 0000 0000 0000 0000 <- castle?

Lets me use bitshifting and bitwise & operations (very efficient) 
to get information about a move
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m) (((m) >> 7) & 0x7F)
#define CAPTURED(m) (((m) >> 14) & 0xF)
#define PROMOTED(m) (((m) >> 20) & 0xF)
// move flags: en passant, pawn start, castle
#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
// Was something captured at all? Was this a capturing move?
// Recall: en passant is a capture (?)
#define MFLAGCAP 0x7C000
// Was the move a promotion?
#define MFLAGPROM 0xF00000

#endif