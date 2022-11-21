// io.c: algebraic notation
#include <stdio.h>
#include "defs.h"

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