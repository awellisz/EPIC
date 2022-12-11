// perft.c ("PERformance Test")
// https://www.chessprogramming.org/Perft

#include <stdio.h>

#include "board.h"
#include "defs.h"
#include "io.h"
#include "makemove.h"
#include "movegen.h"
#include "util.h"

long perft(int depth, board_t *pos) {
    assert(check_board(pos));

    long nodes = 0;
    if (depth == 0) {
        return 1;
    }

    movelist_t list[1];
    generate_all_moves(pos, list);

    int num_moves = list->count;
    for (int i = 0; i < num_moves; i++) {
        // Makes the move and checks the return value
        // undo_move is baked into make_move if it's an illegal move
        if (!make_move(pos, list->moves[i].move)) continue;
        nodes += perft(depth - 1, pos);
        undo_move(pos);
    }

    return nodes;   
}

void perft_divide(int depth, board_t *pos) {
    
    print_board(pos);
    printf("\nStarting test to depth %d\n", depth);

    int start_time = get_time_ms();

    long leaf_nodes = 0;

    movelist_t list[1];
    generate_all_moves(pos, list);

    int move;
    int num_moves = list->count;
    for (int i = 0; i < num_moves; i++) {
       
        move = list->moves[i].move;
        if (!make_move(pos, move)) continue;

        long cum_nodes = leaf_nodes;
        leaf_nodes += perft(depth - 1, pos);
        undo_move(pos);
        long old_nodes = leaf_nodes - cum_nodes;
        printf("Move %d : %s : %ld\n", i + 1, move_to_str(move), old_nodes);
    }

    printf("\nperft complete: %ld nodes in %dms\n", leaf_nodes, get_time_ms() - start_time);

    return;
}