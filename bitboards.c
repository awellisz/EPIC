#include <stdio.h>

#include "defs.h"

// 
const int bit_table[64] = {
    63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 
    29, 2, 51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 
    4, 49, 5, 52, 26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59,
    14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8
};

// Takes a pointer to a bit board
// Takes the first bit (starting at LSB in bb) and returns the 64-based
//   index that this bit was set at (what square) and sets that bit to zero.
// Used for manipulating pawns on their bitboard.
int pop_bit(U64 *bb) {
    U64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    // Zero clue where this constant came from low key
    return bit_table[(fold * 0x783a9b23) >> 26];
}

// Counts the number of bits set to 1 in a bit board
int count_bits(U64 bb) {
    int r;
    for (r = 0; bb; r++, bb &= bb - 1);
    return r;
}

// Clear a bit on the bitboard
void clear_bit(U64 *bb, int sq) {
    *bb &= clear_mask[sq];
}

// Set a bit on the bitboard
void set_bit(U64 *bb, int sq) {
    *bb |= set_mask[sq];
}


// Print a bit board 
void print_bitboard(U64 bb) {
    U64 shift = 1Ull;

    int rank = 0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    printf("\n");
    for(rank = RANK_8; rank >= RANK_1; rank--) {
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            sq64 = SQ64(sq);

            if ((shift << sq64) & bb) {
                printf("X");
            } else {
                printf("-");
            }

        }
        printf("\n");
    }
    printf("\n");
}