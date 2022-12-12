#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "attack.h"
#include "board.h"
#include "data.h"
#include "defs.h"
#include "init.h"
#include "io.h"
#include "makemove.h"
#include "perft.h"

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define START_D "rnbqkbnr/pppppppp/8/8/8/3P4/PPP1PPPP/RNBQKBNR b KQkq - 0 1"
#define START_NO_RQ "8/kppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1"
#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN2 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2"
#define FEN4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN5 "8/3q1p2/8/5P2/4Q3/8/8/8 w - - 0 2"
#define FEN6 "8/8/8/3K4/8/8/8/8 w - - 0 2"
#define PAWNSW "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define PAWNSB "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1"
#define KNIGHTS "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1"
#define ROOKS "8/2k5/3r4/3R4/8/1R4r1/7K/8 b - - 0 1"
#define QUEENS "8/2k5/1n3n2/6q1/6N1/8/1Q5K/8 b - - 0 1"
#define TRICKY "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define PROMOT "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1"


#define MOVE(f,t,ca,pro,fl) ((f) | ((t)<<7) | ((ca) << 14) | ((pro) << 20) | (fl))

// test fxn
void show_atk(const int side, const board_t *pos) {
    int rank = 0;
    int file = 0;
    int sq = 0;

    printf("\n\nSquares attacked by:%c\n", side_char[side]);
    for (rank = RANK_8; rank >= RANK_1; rank--) {
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file, rank);
            if (sq_attacked(sq, side, pos)) {
                printf("X");
            } else {
                printf("-");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

int main() {

    all_init();
    board_t board[1];
    movelist_t list[1];

    parse_fen(START_FEN, board);

    char move_buffer[16];
    char *m_input;
    int move = 0;
    printf("Welcome to EPIC! Enter a move in algebraic notation to begin a game or q to quit.\n");

    while (true) {
        print_board(board);
        printf("> ");
        m_input = fgets(move_buffer, 6, stdin);

        if (m_input[0] == 'q') exit(0);

        // undo move
        if (m_input[0] == 'u') {
            undo_move(board);
        } else if (m_input[0] == 'p') {
            perft_divide(4, board);
        } else {
            move = parse_move(m_input, board);
            if (move != 0) {
                make_move(board, move);
            } else {
                printf("Invalid or illegal move.\n");
            }
        }

        fflush(stdin);
            
    }

    return 0;
}
