#include <stdio.h>
#include <stdbool.h>

#include "defs.h"

// Check if the current position is a repetition of any previous position
// NB: The fifty move rule counter is reset for pawn moves and captures because
//     it's impossible to repeat those moves, so only loop through as many moves
//     as the 50 move counter. (Also, jump by 2's to move through moves and not plies)
bool is_repetition(const board_t *pos) {
    for (int i = 0; i <= pos->fifty_move; i += 2) {
        if (pos->pos_key == pos->history[pos->hist_ply - i].pos_key) {
            return true;
        }
    }
    return false;
}

void search_positions(board_t *pos) {
    return;
}