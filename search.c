#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "defs.h"
#include "search.h"

static void check_up() {
    // Check if time is up or interrupt from GUI
    return;
}

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

// Clear everything to zero, ready for a new search
static void clear_for_search(board_t *pos, searchinfo_t *info) {
    return;
}

// Deal with the horizon effect
static int quiescence(int alpha, int beta, board_t *pos, searchinfo_t *info) {
    return 0;
}

static int alpha_beta(int alpha, int beta, int depth, board_t *pos, 
                      searchinfo_t *info, bool do_null) {
    return 0;
}

void search_positions(board_t *pos, searchinfo_t *info) {
    // Iterative deepening, search initialization
    return;
}