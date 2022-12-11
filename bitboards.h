#ifndef BITBOARDS_H
#define BITBOARDS_H

#include "defs.h"

void print_bitboard(U64 bb);
int pop_bit(U64 *bb);
int count_bits(U64 bb);
void set_bit(U64 *bb, int sq);
void clear_bit(U64 *bb, int sq);

#endif
