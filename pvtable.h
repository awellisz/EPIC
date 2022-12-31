#ifndef PVTABLE_H
#define PVTABLE_H

#include "defs.h"
#include "board.h"

void init_pvtable(pvtable_t *table);
int probe_pvtable(const board_t *pos);
void store_pv_move(const board_t *pos, const int move);
void clear_pvtable(pvtable_t *table);
int get_pv_line(const int depth, board_t *pos);

#endif