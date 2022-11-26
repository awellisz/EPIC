#ifndef DEFS_H
#define DEFS_H

#include <stdlib.h>
// to disable assertions:
#define NDEBUG 
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

//typedef unsigned long long U64;
typedef uint64_t U64;

#define NAME "EPIC" // E.P.I.C. Plays Incredible Chess
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum { BRD_SQ_NUM = 120 };
enum { MAX_GAME_MOVES = 2048 };
enum { MAX_POSITION_MOVES = 256 };
// maximum number of possible moves in a position -- seems to actually be 218

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

// struct containing information to undo a move
typedef struct {
    int move;
    int castle_perm;
    int en_pas;
    int fifty_move;
    U64 pos_key;
} undo_t;

// Board representation 
typedef struct {
    // Keep track of all squares on board and what piece is on each sq
    int pieces[BRD_SQ_NUM];
    // 3 arrays: white, black, and both
    // A bit is set to 1 if there's a pawn of the corresponding color
    //  on that square of the board. This makes it easier to find
    //  things like open files and doubled pawns.
    U64 pawns[3];
    // Keep track of where the white and black kings are
    int king_sq[2];

    // Current side to move
    int side;
    // en passant square, if there is one
    int en_pas;
    // 50 move counter for draws
    int fifty_move;
    // how many half-moves we're into the current search
    int ply;
    // in the total game, how many half-moves have been played
    int hist_ply;
    // castling permissions
    int castle_perm;
    // position key, uniquely generated for each position
    U64 pos_key;

    // number of each piece (there are 13 unique piece types total, incl. EMPTY)
    int pce_num[13];
    // # of big pieces for each color, i.e. anything that isn't a pawn
    int big_pce[2];
    // major pieces, i.e. rooks and queens
    int maj_pce[2];
    // minor pieces, i.e. bishops and knights
    int min_pce[2];
    // value of material
    int material[2];

    // indexed by hist_ply to take back moves
    // also allows us to check for position repetitions for 3-move repeat draws
    undo_t history[MAX_GAME_MOVES];

    // piece list
    // 13 piece types, up to 10 of each piece (e.g. 2 rooks + 8 pawns promo'd)
    // E.g. looking for a white knight:
    //   pce_list[wN][0] = F3; pce_list[wN][1] = D4; pce_list[wN][2] = NO_SQ;
    // Once we hit NO_SQ, we know there are no more of that particular piece on the board
    // Overhead much lower than looping through every square on the board
    int pce_list[13][10];

} board_t;


/************************
 GAME MOVE REPRESENTATION
*************************/

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


/****************
 *    MACROS    *
 ****************/

// Given file and rank, return 120-sq based square number
#define FR2SQ(f, r) ((21 + f) + (r * 10))
#define SQ64(sq120) (Sq120ToSq64[sq120])
#define SQ120(sq64) (Sq64ToSq120[sq64])
// Set or clear a bit on a bitboard (used for pawns)
#define SETBIT(bb,sq) ((bb) |= set_mask[(sq)])
#define CLRBIT(bb,sq) ((bb) &= clear_mask[(sq)])

// For attack.c: check piece type
#define IsBQ(p) (piece_bishop_queen[(p)])
#define IsRQ(p) (piece_rook_queen[(p)])
#define IsN(p) (piece_knight[(p)])
#define IsK(p) (piece_king[(p)])


/****************
 *    GLOBALS   *
 ****************/

// Take an index from 21 to 98 on a 120-sq board representation
//  and return the index on a 64-position based board
extern int Sq120ToSq64[BRD_SQ_NUM];
// same as above, but vice versa
extern int Sq64ToSq120[64];
// set mask, clear mask in init.c
extern U64 set_mask[64];
extern U64 clear_mask[64];
// For hash keys
U64 piece_keys[13][120];
U64 side_key; 
U64 castle_keys[16];
// for print board (data.c)
extern char pce_char[];
extern char side_char[];
extern char rank_char[];
extern char file_char[];
// characteristics of pieces (see data.c)
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
// define in data.c for movegen.c
extern int pce_dir[13][8];
// init.c
extern int files_brd[BRD_SQ_NUM];
extern int ranks_brd[BRD_SQ_NUM];

#endif