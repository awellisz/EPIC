#ifndef SEARCH_H
#define SEARCH_H

#include <stdbool.h> 
#include "board.h"

typedef struct {
    int start_time; // time we start searching
    int stop_time; // 
    int depth;

    bool depth_set; // only search to a certain depth
    bool time_set; // only search for a certain amount of time
    bool moves_to_go; 
    bool infinite; // never stop search until GUI sends stop cmd to engine

    long nodes; // count of all nodes searched
    bool quit; // 
    bool stopped; // true if GUI sends an interrupt that engine is running out of time
} searchinfo_t;

bool is_repetition(const board_t *pos);
void search_positions(board_t *pos, searchinfo_t *info);

#endif