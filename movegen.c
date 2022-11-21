#include <stdio.h>
#include "defs.h"
#include "functions.h"

// Create a move integer from flags etc.
// from, to, capture, promotion, flags
#define MOVE(f,t,ca,pro,fl) ((f) | ((t)<<7) | ((ca) << 14) | ((pro) << 20) | (fl))
// avoids an unnecessary function call:
#define SQOFFBOARD(sq) (files_brd[(sq)]==OFFBOARD)

// Loop for sliding piece move generation
// For white: loop from index zero until you reach value 0
// For black: loop from index four until you reach value 0
int loop_slides[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
// Array to know where to index into loop_slides
// (loop_slides_index)
int ls_index[2] = {0, 4};

// Same for non-sliding:
int loop_nonslides[6] = {wN, wK, 0, bN, bK, 0};
int ln_index[2] = {0, 3}; 



int piece_dir[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    
};

// Add the input move into the movelist's array and updates the count
void add_quiet_move(const board_t *pos, int move, movelist_t *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void add_capture_move(const board_t *pos, int move, movelist_t *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

/* Adds a pawn move with special logic for promotion moves.
 * If no capture: add a quiet move; if capture: add a capture move
 *
 * pos: board position
 * color: the pawn color (for determining where promotion happens)
 * from, to: from and to squares
 * list: the current movelist 
 */
void add_pawn_move(const board_t *pos, const int pawn_col, const int from, 
                         const int to, const int cap, movelist_t *list) {
    
    assert(piece_valid_empty(cap));
    assert(sq_on_board(from));
    assert(sq_on_board(to));

    // Possible pieces to promote to (unknown color)
    int uQ, uR, uB, uN;
    int prom_from_rank;

    switch (pawn_col) {
        case wP:
            prom_from_rank = RANK_7;
            uQ = wQ;
            uR = wR;
            uB = wB;
            uN = wN;
            break;
        case bP:
            prom_from_rank = RANK_2;
            uQ = bQ;
            uR = bR;
            uB = bB;
            uN = bN;
            break;
        default:
            printf("uh oh!\n");
    }

    switch (cap) {
        // Pawn is not capturing anything
        case EMPTY:
            // Starting on 2nd-to-last rank -> next move is a promotion
            if(ranks_brd[from] == prom_from_rank) {
                add_quiet_move(pos, MOVE(from,to,EMPTY,uQ,0), list);
                add_quiet_move(pos, MOVE(from,to,EMPTY,uR,0), list);
                add_quiet_move(pos, MOVE(from,to,EMPTY,uB,0), list);
                add_quiet_move(pos, MOVE(from,to,EMPTY,uN,0), list);
            } else {
                add_quiet_move(pos, MOVE(from,to,EMPTY,EMPTY,0), list);
            }
            break;
        // Pawn is capturing on the last rank and promoting
        default:
            if(ranks_brd[from] == prom_from_rank) {
                add_capture_move(pos, MOVE(from,to,cap,uQ,0), list);
                add_capture_move(pos, MOVE(from,to,cap,uR,0), list);
                add_capture_move(pos, MOVE(from,to,cap,uB,0), list);
                add_capture_move(pos, MOVE(from,to,cap,uN,0), list);
            } else {
                add_capture_move(pos, MOVE(from,to,cap,EMPTY,0), list);
            }
    }
}


// Generates pseudo-legal moves -- checking for illegal moves (e.g. moving
//  a piece that lets your king get in check) will be checked later.
void generate_all_moves(const board_t *pos, movelist_t *list) {
    assert(check_board(pos));

    list->count = 0;
    int pce = EMPTY;
    int side = pos->side;
    int sq = 0;
    int t_sq = 0;
    int pce_num = 0;
    // For pawns:
    int pawn_col, home_rank, opponent_col;
    int fdir, ldir, rdir;
    // For pieces:
    int pce_i; // piece index for loop_slides[] etc.


    switch (side) {
        case WHITE:
            pawn_col = wP;
            fdir = 10; // forward direction
            home_rank = RANK_2;
            ldir = 9; // left capture direction
            rdir = 11; // right capture direction
            opponent_col = BLACK;
            break;
        case BLACK:
            pawn_col = bP;
            fdir = -10;
            home_rank = RANK_7;
            ldir = -9;
            rdir = -11;
            opponent_col = WHITE;
            break;
        default:
            fprintf(stderr, "Error: generate_all_moves\n");
    }

    // Loop through all pawns
    for (pce_num = 0; pce_num < pos->pce_num[pawn_col]; pce_num++) {
        // For each pawn, find what square it's on
        sq = pos->pce_list[pawn_col][pce_num];
        assert(sq_on_board(sq));

        // Generate non-capture moves
        // Can only move forward if empty space in front
        // Moving a pawn forward is +/- 10 on board square number representation
        if (pos->pieces[sq + fdir] == EMPTY) {
            add_pawn_move(pos, pawn_col, sq, sq+fdir, EMPTY, list);
            // First pawn move: either 1 or 2 squares
            if (ranks_brd[sq] == home_rank && pos->pieces[sq + 2*fdir] == EMPTY) {
                // "pawn start" flag is set
                add_quiet_move(pos, MOVE(sq,(sq+2*fdir),EMPTY,EMPTY,MFLAGPS),list);
            }
        }
        // Pawns capture in +9/+11 or -9/-11 directions. If the attacked square 
        // is on the board and has opponent's piece, add the move.
        if (!SQOFFBOARD(sq+ldir) && piece_col[pos->pieces[sq+ldir]] == opponent_col) {
            add_pawn_move(pos, pawn_col, sq, sq+ldir, pos->pieces[sq+ldir], list);
        }
        if (!SQOFFBOARD(sq+rdir) && piece_col[pos->pieces[sq+rdir]] == opponent_col) {
            add_pawn_move(pos, pawn_col, sq, sq+rdir, pos->pieces[sq+rdir], list);
        }
        // En passant move
        if (!SQOFFBOARD(sq+fdir) && (sq+ldir == pos->en_pas)) {
            add_capture_move(pos, MOVE(sq, sq+ldir, EMPTY, EMPTY, MFLAGEP), list);
        } else if (!SQOFFBOARD(sq+rdir) && (sq+rdir == pos->en_pas)) {
            add_capture_move(pos, MOVE(sq, sq+rdir, EMPTY, EMPTY, MFLAGEP), list);
        }
    }

    //////////////////////////////////
    // CONSIDER LOOPING BACKWARS SO THAT 
    // WE GENERATE MOVES FOR HIGH-VALUE PIECES FIRST --
    // THIS WILL BE IMPORTANT FOR ALPHA BETA PRUNING LATER
    ////////////////////////////////////

    // SLIDING PIECES (BISHOP, ROOK, QUEEN)
    int dir = 0;
    pce_i = ls_index[side]; //piece index
    pce = loop_slides[pce_i++];
    while (pce != 0) {
        assert(piece_valid(pce));
        printf("Sliding Piece Index: %d, piece: %d\n", pce_i, pce);
        for (pce_num = 0; pce_num < pos->pce_num[pce]; pce_num++) {

            sq = pos->pce_list[pce][pce_num];
            assert(sq_on_board(sq));
            printf("Piece: %c on %s\n", pce_char[pce], sq_to_str(sq));

            for (int i = 0; pce_dir[pce][i] && i < 8; i++) {
                dir = pce_dir[pce][i];
                t_sq = sq + dir;

                // Loop along the "ray" in each direction
                while(!SQOFFBOARD(t_sq)) {
                    // recall: BLACK = 0, WHITE = 1
                    if (pos->pieces[t_sq] != EMPTY) {
                        if (piece_col[pos->pieces[t_sq]] == (side ^ 1)) {
                            printf("\t\tCapture on %s\n", sq_to_str(t_sq));
                        }
                        // break out if we hit a non-empty square
                        break;
                    }
                    printf("\t\tNormal on %s\n", sq_to_str(t_sq));
                    t_sq += dir;
                }
            }
        }
        pce = loop_slides[pce_i++];
    }
    

    // NON-SLIDING PIECES (KNIGHT AND KING)
    pce_i = ln_index[side]; //piece index
    pce = loop_nonslides[pce_i++];
    while (pce != 0) {
        assert(piece_valid(pce));
        printf("Nonsliding piece index: %d, piece: %d\n", pce_i, pce);

        for (pce_num = 0; pce_num < pos->pce_num[pce]; pce_num++) {

            sq = pos->pce_list[pce][pce_num];
            assert(sq_on_board(sq));
            printf("Piece: %c on %s\n", pce_char[pce], sq_to_str(sq));

            for (int i = 0; pce_dir[pce][i] && i < 8; i++) {
                dir = pce_dir[pce][i];
                t_sq = sq + dir;

                if(SQOFFBOARD(t_sq)) continue;

                // recall: BLACK = 0, WHITE = 1
                if (pos->pieces[t_sq] != EMPTY) {
                    if (piece_col[pos->pieces[t_sq]] == (side ^ 1)) {
                        printf("\t\tCapture on %s\n", sq_to_str(t_sq));
                    }
                    continue;
                }
                printf("\t\tNormal on %s\n", sq_to_str(t_sq));
            }
        }
        pce = loop_nonslides[pce_i++];
    }
}
