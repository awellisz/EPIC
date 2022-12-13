// pvtable.c
// 12/12/22

#include <stdio.h>
#include <assert.h>

#include "board.h"
#include "defs.h"
#include "pvtable.h"

const int pv_size = 0x100000 * 2; // 2 megabytes to allocate

void clear_pvtable(pvtable_t *table) {
    pventry_t *pv_entry;
    // Step through the pointers in memory
    for (pv_entry = table->p_table; pv_entry < (table->p_table + table->num_entries); pv_entry++) {
        pv_entry->pos_key = 0ULL;
        pv_entry->move = NOMOVE;
    }
}

void init_pvtable(pvtable_t *table) {

    table->num_entries = pv_size / sizeof(pventry_t);
    // Make sure we don't go index out of bounds
    table->num_entries -= 2;
    // p_table pointer is initialized to NULL
    if (table->p_table != NULL) free(table->p_table);
    table->p_table = (pventry_t *) malloc(table->num_entries * sizeof(pventry_t));
    clear_pvtable(table);
    printf("pvtable initialized: %d entries\n", table->num_entries);
}

void store_pv_move(const board_t *pos, const int move) {
    // Give an index between 0 and (num_entries - 1)
    // Should be unique for each position key
    int index = pos->pos_key % pos->pvtable->num_entries;
    assert(index >= 0 && index <= pos->pvtable->num_entries - 1);

    pos->pvtable->p_table[index].move = move;
    pos->pvtable->p_table[index].pos_key = pos->pos_key;
}

// Retrieve a move from the PV table
int probe_pvtable(const board_t *pos) {
    // Get hash of position key
    // Hash collisions are possible, dealt with later
    int index = pos->pos_key % pos->pvtable->num_entries;
    assert(index >= 0 && index <= pos->pvtable->num_entries - 1);

    if (pos->pvtable->p_table[index].pos_key == pos->pos_key) {
        return pos->pvtable->p_table[index].move;
    }

    // If nothing found, return 0
    return NOMOVE;
}