// makemove.c
// 11/21/22
#include <stdio.h>
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
    assert(sq_on_board(sq))
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
        pos->big_pce[col]--;
        if (piece_maj[pce]) pos->maj_pce[col]--;
        else pos->min_pce[col]--;
    } else {
        // clear the sq on the pawn bitboard
        CLRBIT(pos->pawns[col],SQ64(sq));
        CLRBIT(pos->pawns[BOTH],SQ64(sq));
    }


    for (int i = 0; i < pos->pce_num[pce]; i++) {
        if (pos->pce_list[pce][i] == sq) {
            t_pce_num = i;
            break;
        }
    }
    // Double check that we did indeed find the piece to remove in the array
    assert(t_pce_num != -1);
    // Decrement number of pieces in the position's piece list
    pos->pce_num[pce]--;
    // Set the removed entry to the final entry
    pos->piece_list[pce][t_pce_num] = pos->piece_list[pce][pos->pce_num[pce]];
    // So: 
    // - remove the piece at index t_pce_num in the piece list
    // - replace it with the last piece in the list
    // - reduce the piece list length so that it stops at the new end
    // - (the order of the pieces doesn't matter so this is valid)

}