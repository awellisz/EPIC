#ifndef INIT_H
#define INIT_H

void all_init();

extern int files_brd[BRD_SQ_NUM];
extern int ranks_brd[BRD_SQ_NUM];

extern U64 set_mask[64];
extern U64 clear_mask[64];

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];

extern U64 piece_keys[13][120];
extern U64 side_key; 
extern U64 castle_keys[16];

// Given file and rank, return 120-sq based square number
#define FR2SQ(f, r) ((21 + f) + (r * 10))
#define SQ64(sq120) (Sq120ToSq64[sq120])
#define SQ120(sq64) (Sq64ToSq120[sq64])
// Set or clear a bit on a bitboard (used for pawns)
#define SETBIT(bb,sq) ((bb) |= set_mask[(sq)])
#define CLRBIT(bb,sq) ((bb) &= clear_mask[(sq)])

#endif
