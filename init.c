/* October, 2022
 * Initializes the chess engine
 */

#include "defs.h"
#include "functions.h"

int Sq120ToSq64[BRD_SQ_NUM];
int Sq64ToSq120[64];

U64 set_mask[64];
U64 clear_mask[64];

// Used for generating hash keys for each position
U64 piece_keys[13][120];
U64 side_key; // Hash in a random number if it's white to move
U64 castle_keys[16]; // 16 castle keys bc 4 bits -> 16 possible numbers

// arrays to see what rank or file a square is
int files_brd[BRD_SQ_NUM];
int ranks_brd[BRD_SQ_NUM];

void init_files_ranks_brd() {
    int file, rank, sq;
    // initialize all squares to OFFBOARD
    for (int i = 0; i < BRD_SQ_NUM; i++) {
        files_brd[i] = OFFBOARD;
        ranks_brd[i] = OFFBOARD;
    }
    // 
    for (rank = RANK_1; rank <= RANK_8; rank++) {
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            files_brd[sq] = file;
            ranks_brd[sq] = rank;
        }
    }
}

void init_hashkeys() {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 120; j++) {
            piece_keys[i][j] = rand_U64();
        }
    }

    side_key = rand_U64();
    
    for (int i = 0; i < 16; i++) {
        castle_keys[i] = rand_U64();
    }
}

void init_bit_masks() {
    for(int i = 0; i < 64; i++) {
        set_mask[i] = 0ULL;
        clear_mask[i] = 0ULL;
    }
    for(int i = 0; i < 64; i++) {
        set_mask[i] |= (1ULL << i);
        clear_mask[i] = ~set_mask[i];
    }
}

void init_sq_120_to_64() {

    int sq = A1;
    int sq64 = 0;

    for(int i = 0; i < BRD_SQ_NUM; i++) {
        Sq120ToSq64[i] = 65;
    }

    for(int i = 0; i < 64; i++) {
        Sq64ToSq120[i] = 120;
    }

    int r, f;

    for (r = RANK_1; r <= RANK_8; r++) {
        for (f = FILE_A; f <= FILE_H; f++) {
            sq = FR2SQ(f, r); 
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

/* Called at the start of main() in epic.c */
void all_init() {
    init_sq_120_to_64();
    init_bit_masks();
    init_hashkeys();
    init_files_ranks_brd();
}
