#ifndef PVTABLE_H
#define PVTABLE_H

#include "defs.h"

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

void init_pvtable(pvtable_t *table);

#endif