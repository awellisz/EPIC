// hashkeys.c

#include "board.h"
#include "defs.h"
#include "init.h"
#include "hashkeys.h"

U64 rand_U64(void) {
    U64 num1 = (U64) rand();
    U64 num2 = (U64) rand();
    return num1 | (num2 << 32);
}


U64 generate_pos_key(const board_t *pos) {
    int sq = 0;
    U64 final_key = 0;
    int piece = EMPTY;

    // Loop through all squares 
    for (sq = 0; sq < BRD_SQ_NUM; sq++) {
        // Set 'piece' to whatever value of the pos array at that square
        piece = pos->pieces[sq];
        // Check that the piece is actually on the board and a real piece
        if ((piece != NO_SQ) && (piece != EMPTY) && (piece != OFFBOARD)) {
            assert((piece >= wP) && (piece <= bK));
            // Hash in (xor) whatever number is in the piece_keys array
            final_key ^= piece_keys[piece][sq]; 
        }
    }

    // If it's white to move, hash in the side_key
    if (pos->side == WHITE) {
        final_key ^= side_key;
    }
    // If there's an en passant square, hash it in
    if (pos->en_pas != NO_SQ) {
        assert((pos->en_pas >= 0) && (pos->en_pas < BRD_SQ_NUM));
        // Use the zeroeth index (EMPTY)
        final_key ^= piece_keys[EMPTY][pos->en_pas];
    }

    assert((pos->castle_perm >= 0) && (pos->castle_perm <= 15));
    final_key ^= castle_keys[pos->castle_perm];

    return final_key;
}