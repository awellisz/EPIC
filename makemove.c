// makemove.c
// 11/21/22
#include <stdio.h>
#include <stdbool.h>
#include "defs.h"
#include "functions.h"

// Hashing macros: assume board position pointer is called "pos"
#define HASH_PCE(pce, sq) (pos->pos_key ^= (piece_keys[(pce)][(sq)]))
#define HASH_CA (pos->pos_key ^= (castle_keys[(pos->castle_perm)]))
#define HASH_SIDE (pos->pos_key ^= (side_key))
#define HASH_EP (pos->pos_key ^= (piece_keys[EMPTY][(pos->en_pas)]))

// not to be confused with castle_perm in undo_t or board_t
// Recall: castle perms start with 1111 = 0xF
// represented by:
// enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };
// If white loses all castling, that is represented as 0011 
// So, if e.g. white moves from E1, that means the king must have moved, 
// and all castling permissions are lost. This is done by &'ing the overall
// castling permissions by 3, so 1111 & 0011 -> 0011 (i.e. no white castle perms)
// Most squares are 0xF (1111) because they don't affect castle perms.
// Similarly, moving the rook on A1 removes white's queenside castling perms, etc.
// Only non-0xF squares: A8, E8, H8, A1, E1, H1
const int castle_perms[120] = {
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF,  13, 0xF, 0xF, 0xF,  12, 0xF, 0xF,  14, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF,   7, 0xF, 0xF, 0xF,   3, 0xF, 0xF,  11, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF,
    0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF
};

// Clear the piece on the given square
static void clear_piece(const int sq, board_t *pos) {
    assert(sq_on_board(sq));
    // Get the piece currently on the square
    int pce = pos->pieces[sq];
    assert(piece_valid(pce));

    int col = piece_col[pce];
    int t_pce_num = -1;

    // xor the piece on that square (i.e. hash it OUT of the position key)
    HASH_PCE(pce,sq);
    // set the square to empty, decrement material count for that color
    pos->pieces[sq] = EMPTY;
    pos->material[col] -= piece_val[pce];

    // If it's not a pawn:
    if (piece_big[pce]) {
        pos->big_pce[col] -= 1;
        if (piece_maj[pce]) {
            pos->maj_pce[col] -= 1;
        } 
        else {
            pos->min_pce[col] -= 1;
        }
    } else {
        // clear the sq on the pawn bitboard if it's a pawn
        CLRBIT(pos->pawns[col],SQ64(sq));
        CLRBIT(pos->pawns[BOTH],SQ64(sq));
    }


    // - find the index of the piece to remove
    // - replace piece to remove with the last piece in the list
    // - reduce the piece list length so that it stops at the new end
    // - (the order of the pieces doesn't matter so this is valid)
    for (int i = 0; i < pos->pce_num[pce]; i++) {
        if (pos->pce_list[pce][i] == sq) {
            t_pce_num = i;
            break;
        }
    }
    assert(t_pce_num != -1);
    pos->pce_num[pce] -= 1;
    pos->pce_list[pce][t_pce_num] = pos->pce_list[pce][pos->pce_num[pce]];

}


static void add_piece(const int sq, board_t *pos, const int pce) {
    assert(piece_valid(pce));
    assert(sq_on_board(sq));

    int col = piece_col[pce];

    // Hash the new piece INTO the poskey
    HASH_PCE(pce,sq);
    pos->pieces[sq] = pce;

    if (piece_big[pce]) {
        pos->big_pce[col]++;
        if (piece_maj[pce]) pos->maj_pce[col]++;
        else pos->min_pce[col]++;
    } else {
        SETBIT(pos->pawns[col],SQ64(sq));
        SETBIT(pos->pawns[BOTH],SQ64(sq));
    }

    pos->material[col] += piece_val[pce];
    pos->pce_list[pce][pos->pce_num[pce]++] = sq;

}

// Move a piece from the from square to the to square
// Could be implemented as successive calls to clear_piece() and add_piece(),
//  but this is more efficient.
static void move_piece(const int from, const int to, board_t *pos) {
    assert(sq_on_board(from));
    assert(sq_on_board(to));

    int pce = pos->pieces[from];
    int col = piece_col[pce];

    // For debugging
    bool t_piece_num = false;

    HASH_PCE(pce,from);
    pos->pieces[from] = EMPTY;

    HASH_PCE(pce,to);
    pos->pieces[to] = pce;

    // for pawns
    if (!piece_big[pce]) {
        CLRBIT(pos->pawns[col],SQ64(from));
        CLRBIT(pos->pawns[BOTH],SQ64(from));
        SETBIT(pos->pawns[col],SQ64(to));
        SETBIT(pos->pawns[BOTH],SQ64(to));
    }
    // Change the square for the piece in the piece list
    for (int i = 0; i < pos->pce_num[pce]; i++) {
        if (pos->pce_list[pce][i] == from) {
            pos->pce_list[pce][i] = to;
            t_piece_num = true;
            break;
        }
    }
    assert(t_piece_num);
}


// Undoes the most recent move made by make_move()
void undo_move(board_t *pos) {
    assert(check_board(pos));

    pos->hist_ply -= 1;
    pos->ply -= 1;

    int move = pos->history[pos->hist_ply].move;
    int from = FROMSQ(move);
    int to = TOSQ(move);

    assert(sq_on_board(from));
    assert(sq_on_board(to));

    // Reset castling permissions, fifty move rule, and en passant square
    pos->castle_perm = pos->history[pos->hist_ply].castle_perm;
    pos->fifty_move = pos->history[pos->hist_ply].fifty_move;
    pos->en_pas = pos->history[pos->hist_ply].en_pas;

    // Add back in pawns captured en passant
    if (move & MFLAGEP) {
        if (pos->side == WHITE) add_piece(to-10, pos, bP);
        else add_piece(to+10, pos, wP);
    } else if (move & MFLAGCA) { 
        // Castle move: move the corresponding rook as well
        switch (to) {
            case C1: move_piece(A1, D1, pos); break;
            case G1: move_piece(H1, F1, pos); break;
            case C8: move_piece(A8, D8, pos); break;
            case G8: move_piece(H8, F8, pos); break;
            default: assert(false);
        }
    }

    // NB: We're taking the piece back from the TO sq to the original FROM sq
    // so instead of move_piece(from, to, pos), we do:
    move_piece(to, from, pos);

    // Set king back to 'from' square
    if (piece_king[pos->pieces[from]]) {
        pos->king_sq[pos->side] = from;
    }
    
    // Undo a capture, if any
    int captured = CAPTURED(move);
    if (captured != EMPTY) {
        assert(piece_valid(captured));
        add_piece(to, pos, captured);
    }
    // Undo a promotion, if any
    int promoted = PROMOTED(move);
    if (promoted != EMPTY) {
        assert(piece_valid(promoted) && !piece_pawn[promoted]);
        // The piece that was just created in promotion must be removed from
        //  the 7th/2nd rank and replaced with a pawn
        clear_piece(from, pos);
        add_piece(from, pos, (piece_col[promoted] == WHITE ? wP : bP));
    }
    
    // Change back the side
    pos->side ^= 1;
    // Replace hash with previous position hash
    pos->pos_key = pos->history[pos->hist_ply].pos_key;

    assert(check_board(pos));
}


/* make_move: make a move on the board for the given position (pos)
returns: true if move is valid, false if invalid (i.e. if making the move 
         puts your own king into check)
*/
bool make_move(board_t *pos, int move) {
    assert(check_board(pos));

    int from = FROMSQ(move);
    int to = TOSQ(move);
    int side = pos->side;

    assert(sq_on_board(from));
    assert(sq_on_board(to));
    assert(side_valid(side));
    assert(piece_valid(pos->pieces[from]));

    // Store the hash key of the current position
    // recall: hist_ply is the number of halfmoves played in the entire game
    // recall: history[] is an array of undo_t structs which have pos_key
    pos->history[pos->hist_ply].pos_key = pos->pos_key;

    // Check if move is an en passant move
    if (move & MFLAGEP) {
        // Target square direction depends on side
        // WHITE is 0, BLACK is 1
        int ep_sq = side ? to + 10 : to - 10;
        clear_piece(ep_sq, pos);
    } else if (move & MFLAGCA) { 
        // Castle move: move the corresponding rook as well
        switch (to) {
            case C1: move_piece(A1, D1, pos); break;
            case G1: move_piece(H1, F1, pos); break;
            case C8: move_piece(A8, D8, pos); break;
            case G8: move_piece(H8, F8, pos); break;
            default: assert(false);
        }
    }
    // If there was an en passant square, hash it out of the key
    if (pos->en_pas != NO_SQ) HASH_EP;
    // Hash out the current the castling permission
    HASH_CA;

    // Update history
    pos->history[pos->hist_ply].move = move;
    pos->history[pos->hist_ply].fifty_move = pos->fifty_move;
    pos->history[pos->hist_ply].en_pas = pos->en_pas;
    pos->history[pos->hist_ply].castle_perm = pos->castle_perm;

    // update castling permissions
    // Recall: & has no effect if from or to squares don't matter for castling
    // Recall: castle_perms is the array defined above; castle_perm is an integer
    //         in the board_t struct
    pos->castle_perm &= castle_perms[from];
    pos->castle_perm &= castle_perms[to];
    pos->en_pas = NO_SQ;

    // Hash back in the new castling permissions
    HASH_CA;

    pos->fifty_move++;
    pos->hist_ply++;
    pos->ply++;

    // Get captured piece, if any
    int captured = CAPTURED(move);
    if (captured != EMPTY) {
        assert(piece_valid(captured));
        clear_piece(to, pos);
        // Fifty move reset to zero when a capture is made
        pos->fifty_move = 0;
    }

    // Pawn moves
    if (piece_pawn[pos->pieces[from]]) {
        // moving a pawn resets the 50 move rule
        pos->fifty_move = 0;
        // Check for pawn start
        if (move & MFLAGPS) {
            if (side == WHITE) {
                pos->en_pas = from + 10;
                assert(ranks_brd[pos->en_pas] == RANK_3);
            } else {
                pos->en_pas = from - 10;
                assert(ranks_brd[pos->en_pas] == RANK_6);
            }
            // Hash in the en passant square
            HASH_EP;
        }
    }
    
    // Now that we've cleared all pieces that have been captured off (and dealt
    //  with castling, en passant, etc.), move the actual piece in question.
    move_piece(from, to, pos);

    // Check for and deal with promotion
    int promoted_pce = PROMOTED(move);
    if (promoted_pce != EMPTY) {
        assert(piece_valid(promoted_pce) && !piece_pawn[promoted_pce]);
        clear_piece(to, pos); // clear the pawn
        add_piece(to, pos, promoted_pce); // add the new piece
    }

    // If it was a king that moved, update the position king_sq
    // Probably reduntant, remove later?
    if(piece_king[pos->pieces[to]]) {
        pos->king_sq[pos->side] = to;
    }

    // Change the side 
    pos->side ^= 1;
    HASH_SIDE; // hash in the side

    assert(check_board(pos));

    // Check if move is illegal (recall: pos->side is now opponent; side stays the same)
    if (sq_attacked(pos->king_sq[side], pos->side, pos)) {
        
        //print_board(pos);
        printf("Move %s: king on square %s under attack (illegal)\n", move_to_str(move), sq_to_str(pos->king_sq[side]));
        undo_move(pos);
        return false;
    }

    return true;
}