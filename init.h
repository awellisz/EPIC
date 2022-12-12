#ifndef INIT_H
#define INIT_H

void all_init();

int files_brd[BRD_SQ_NUM];
int ranks_brd[BRD_SQ_NUM];

U64 set_mask[64];
U64 clear_mask[64];

// Take an index from 21 to 98 on a 120-sq board representation
//  and return the index on a 64-position based board
int Sq120ToSq64[BRD_SQ_NUM];
// same as above, but vice versa
int Sq64ToSq120[64];

// Used for generating hash keys for each position
U64 piece_keys[13][120];
U64 side_key; // Hash in a random number if it's white to move
U64 castle_keys[16]; // 16 castle keys bc 4 bits -> 16 possible numbers

#endif
