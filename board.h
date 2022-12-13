#ifndef BOARD_H 
#define BOARD_H 

#include "defs.h"

// struct containing information to undo a move
typedef struct {
    int move;
    int castle_perm;
    int en_pas;
    int fifty_move;
    U64 pos_key;
} undo_t;

// Principle variation entry
// When a move is found in the search that beats alpha (best move),
// store the move and the unique position key that leads to it.
typedef struct {
    U64 pos_key;
    int move;
} pventry_t;

// Pointer is allocated and realloc'd
typedef struct {
    pventry_t *p_table;
    int num_entries;
} pvtable_t;

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

    pvtable_t pvtable[1];
    int pv_array[MAX_DEPTH];

} board_t;

void reset_board(board_t *pos);
void parse_fen(char *fen, board_t *pos);
void print_board(const board_t *pos);
void update_lists_material(board_t *pos);
bool check_board(const board_t *pos);

#endif
