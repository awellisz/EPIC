// io.c: algebraic notation
#include <stdio.h>
#include <assert.h>

#include "defs.h"
#include "movegen.h"
#include "validate.h"

// Convert a (numeric) square to its algebraic notation version
char *sq_to_str(const int sq) {
    // static can be accessed outside the scope of this function
    static char sq_str[3];
    int file = files_brd[sq];
    int rank = ranks_brd[sq];
    // Take advantage of ASCII here
    sprintf(sq_str, "%c%c", ('a'+file), ('1'+rank));
    return sq_str;
}

// Convert a move to algebraic notation
char *move_to_str(const int move) {
    static char mv_str[6];

    int ff = files_brd[FROMSQ(move)]; // file from
    int rf = ranks_brd[FROMSQ(move)]; // rank from
    int ft = files_brd[TOSQ(move)]; // file to
    int rt = ranks_brd[TOSQ(move)]; // rank to
    int prom = PROMOTED(move);

    // Find what move is promoted
    if (prom) {
        char pchar = 'q'; // char for promoted piece
        if (IsN(prom)) {
            pchar = 'n';
        } else if (IsRQ(prom) && !IsBQ(prom)) {
            pchar = 'r';
        } else if (!IsRQ(prom) && IsBQ(prom)) {
            pchar = 'b';
        }
        sprintf(mv_str, "%c%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else {
        sprintf(mv_str, "%c%c%c%c", ('a'+ff), ('1'+rf), ('a'+ft), ('1'+rt));
    }
    return mv_str;
}

void print_move_list(const movelist_t *list) {
    int i = 0, score = 0, move = 0;
    for (i = 0; i < list->count; i++) {
        move = list->moves[i].move;
        score = list->moves[i].score;
        printf("Move %d: %s (score: %d)\n", i+1, move_to_str(move), score);
    }
    printf("movelist: %d moves\n", list->count);
}

// Returns a move based on command line input string
// Returns zero if no/invalid move
int parse_move(char *movep, board_t *pos) {
    // Move must be of the format e.g. a2b3 with valid chess squares
    if (movep[0] < 'a' || movep[0] > 'h') return 0;
    if (movep[1] < '1' || movep[1] > '8') return 0;
    if (movep[2] < 'a' || movep[2] > 'h') return 0;
    if (movep[3] < '1' || movep[3] > '8') return 0;

    // Get numerical values for file and rank by subtracting ASCII values
    int from = FR2SQ((movep[0] - 'a'),(movep[1] - '1'));
    int to = FR2SQ((movep[2] - 'a'),(movep[3] - '1'));

    assert(sq_on_board(from));
    assert(sq_on_board(to));

    // Loop through all possible legal moves and find which one matches input
    movelist_t list[1];
    generate_all_moves(pos, list);

    int move = 0;
    for (int i = 0; i < list->count; i++) {
        move = list->moves[i].move;
        if (FROMSQ(move) == from && TOSQ(move) == to) {
            int prom = PROMOTED(move);
            if (prom != EMPTY) {
                if ((IsRQ(prom) && !IsBQ(prom) && movep[4] == 'r')
                    || (!IsRQ(prom) && IsBQ(prom) && movep[4] == 'b')
                    || (IsRQ(prom) && IsBQ(prom) && movep[4] == 'q')
                    || (IsN(prom) && movep[4] == 'n')) {
                    return move;
                }
                continue;
            }
            return move;
        }
    }
    return 0;
}