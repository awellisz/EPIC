// pvtable.c
// 12/12/22

#include <stdio.h>
#include <assert.h>

#include "board.h"
#include "defs.h"
#include "pvtable.h"

int get_pv_line(const int depth, board_t *pos) {
    assert(depth < MAX_DEPTH);
    // recall: returns NOMOVE if no move found
    int move = probe_pvtable(pos);
    int count = 0;
    while (move != NOMOVE && count < depth) {
        // If the move exists, make the move, add the move into the PV array,
        // and increment the count.
        if (move_exists(pos, move)) {
            make_move(pos, move);
            pos->pv_array[count++] = move;
        } else {
            break;
        }
        // Probe again 
        move = probe_pvtable(pos);
    }

    // Undo all moves to get back to original position
    while (pos->ply > 0) {
        undo_move(pos);
    }

    return count;
}

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

    pos->pvtable->p_table[index].move = move;
    pos->pvtable->p_table[index].pos_key = pos->pos_key;
}

// Retrieve a move from the PV table
int probe_pvtable(const board_t *pos) {
    // Get hash of position key
    // Hash collisions are possible, dealt with later
    int index = pos->pos_key % pos->pvtable->num_entries;

    if (pos->pvtable->p_table[index].pos_key == pos->pos_key) {
        return pos->pvtable->p_table[index].move;
    }

    // If nothing found, return 0
    return NOMOVE;
}