// pvtable.c
// 12/12/22

#include <stdio.h>

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