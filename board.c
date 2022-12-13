#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "bitboards.h"
#include "board.h"
#include "data.h"
#include "defs.h"
#include "hashkeys.h"
#include "init.h"
#include "pvtable.h"

bool check_board(const board_t *pos) {
    // temporary variables to check all these values
    // If these calculated values are the same as the stored values
    //  for the board, then the board passes this check.

    int t_pce_num[13] = {0};
    int t_big_pce[2] = {0};
    int t_maj_pce[2] = {0};
    int t_min_pce[2] = {0};
    int t_material[2] = {0};

    int t_num, t_piece, sq120, sq64, color, pcount;

    U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

    t_pawns[WHITE] = pos->pawns[WHITE];
    t_pawns[BLACK] = pos->pawns[BLACK];
    t_pawns[BOTH] = pos->pawns[BOTH];

    // Check piece lists: loop by piece type...
    for (t_piece = wP; t_piece < bK; t_piece++) {
        // ... and by piece number for each type
        for (t_num = 0; t_num < pos->pce_num[t_piece]; t_num++) {
            sq120 = pos->pce_list[t_piece][t_num];
            assert(pos->pieces[sq120] == t_piece);
        }
    }

    // Check piece count + other counters
    // Go through the board and count the number of pieces by type, color
    for (sq64 = 0; sq64 < 64; sq64++) {
        sq120 = SQ120(sq64);
        t_piece = pos->pieces[sq120];
        t_pce_num[t_piece]++;
        color = piece_col[t_piece];
        if (piece_big[t_piece]) t_big_pce[color]++;
        if (piece_min[t_piece]) t_min_pce[color]++;
        if (piece_maj[t_piece]) t_maj_pce[color]++;

        t_material[color] += piece_val[t_piece];
    }

    // Make sure the number of pieces found on the board and stored in 
    //  the temp piece number array equals, for each piece type, the 
    // corresponding array in pos
    for (t_piece = wP; t_piece <= bK; t_piece++) {
        assert(t_pce_num[t_piece] == pos->pce_num[t_piece]);
    }

    // Check pawn bitboard counts
    pcount = count_bits(t_pawns[WHITE]);
    assert(pcount == pos->pce_num[wP]);
    pcount = count_bits(t_pawns[BLACK]);
    assert(pcount == pos->pce_num[bP]);
    pcount = count_bits(t_pawns[BOTH]);
    assert(pcount == (pos->pce_num[bP] + pos->pce_num[wP]));

    // Check bitboard squares
    while (t_pawns[WHITE]) {
        sq64 = pop_bit(&t_pawns[WHITE]);
        assert(pos->pieces[SQ120(sq64)] == wP);
    }
    while (t_pawns[BLACK]) {
        sq64 = pop_bit(&t_pawns[BLACK]);
        assert(pos->pieces[SQ120(sq64)] == bP);
    }
    while (t_pawns[BOTH]) {
        sq64 = pop_bit(&t_pawns[BOTH]);
        assert((pos->pieces[SQ120(sq64)] == wP) || (pos->pieces[SQ120(sq64)] == bP));
    }


    // sanity checks
    assert(t_material[WHITE] =- pos->material[WHITE]);
    assert(t_material[BLACK] == pos->material[BLACK]);
    assert(t_maj_pce[WHITE] == pos->maj_pce[WHITE]);
    assert(t_maj_pce[BLACK] == pos->maj_pce[BLACK]);
    assert(t_min_pce[WHITE] == pos->min_pce[WHITE]);
    assert(t_min_pce[BLACK] == pos->min_pce[BLACK]);
    assert(t_big_pce[WHITE] == pos->big_pce[WHITE]);
    assert(t_big_pce[BLACK] == pos->big_pce[BLACK]);

    assert(pos->side==WHITE || pos->side == BLACK);
    assert(generate_pos_key(pos) == pos->pos_key);
    // either en passant square doesn't exist, or if it does, if it's white to move,
    // en_pas must be on rank 6, and if it's black to move, it must be on rank 3
    assert(pos->en_pas == NO_SQ || (ranks_brd[pos->en_pas] == RANK_6 && pos->side == WHITE)
            || (ranks_brd[pos->en_pas] == RANK_3 && pos->side == BLACK));
    
    //assert(pos->pieces[pos->king_sq[WHITE]] == wK);
    //assert(pos->pieces[pos->king_sq[BLACK]] == bK);

    return true;
}

void update_lists_material(board_t *pos) {
    int piece, sq, color;

    for (int i = 0; i < BRD_SQ_NUM; i++) {
        sq = i;
        piece = pos->pieces[i];
        if ((piece != OFFBOARD) && (piece != EMPTY)) { 
            // why piece != OFFBOARD ?
            // changing to "sq != OFFBOARD" caused a segmentation fault lol
            // check this later
            color = piece_col[piece];

            // if the piece is big, increment the board's big piece counter. etc.
            if (piece_big[piece]) pos->big_pce[color]++;
            if (piece_min[piece]) pos->min_pce[color]++;
            if (piece_maj[piece]) pos->maj_pce[color]++;

            pos->material[color] += piece_val[piece];

            // recall: int pce_list[13][10];
            // e.g. first white pawn is on a2: pce_list[wP][0] = a2;
            pos->pce_list[piece][pos->pce_num[piece]] = sq;
            pos->pce_num[piece] += 1;

            if (piece == wK) pos->king_sq[WHITE] = sq;
            if (piece == bK) pos->king_sq[BLACK] = sq;

            if(piece == wP) {
                // Setting bit in 64 indexing format but sq is in 
                //  120 indexing format
                SETBIT(pos->pawns[WHITE],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            } else if (piece == bP) {
                SETBIT(pos->pawns[BLACK],SQ64(sq));
                SETBIT(pos->pawns[BOTH],SQ64(sq));
            }
        }
    }
}

// Parse a fen string, set up the position on the board pos
void parse_fen(char *fen, board_t *pos) {
    assert(fen != NULL);
    assert(pos != NULL);

    // FEN strings start at rank 8, file A
    int rank = RANK_8;
    int file = FILE_A;
    int piece = 0;
    int count = 0;
    int sq64 = 0;
    int sq120 = 0;

    reset_board(pos);

    while ((rank >= RANK_1) && *fen) {
        count = 1;
        // Switch on current character we're looking at
        switch (*fen) {
            case 'p': piece = bP; break;
            case 'r': piece = bR; break;
            case 'n': piece = bN; break;
            case 'b': piece = bB; break;
            case 'k': piece = bK; break;
            case 'q': piece = bQ; break;
            case 'P': piece = wP; break;
            case 'R': piece = wR; break;
            case 'N': piece = wN; break;
            case 'B': piece = wB; break;
            case 'K': piece = wK; break;
            case 'Q': piece = wQ; break;
            // If it's not a piece, it must be some int num empty sqrs
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                // subtract ASCII values from characters to get integer val
                count = *fen - '0';
                break;
            // Either at a new rank or at the end of the pieces
            case '/':
            case ' ':
                rank--;
                file = FILE_A;
                fen++;
                continue;
            default:
                fprintf(stderr, "parse_fen error\n");
                return;
        }

        for (int i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
            sq120 = SQ120(sq64);
            if (piece != EMPTY) {
                pos->pieces[sq120] = piece;
            }
            file++;
        }
        // Increment to read the next character
        fen++;
    }

    // At this point: read all pieces, fen ptr now pointing at side
    assert(*fen == 'w' || *fen == 'b');
    pos->side = (*fen == 'w') ? WHITE : BLACK;
    fen += 2;

    // Castling perms
    // Anyonwhere from 1 (-) to 4 (KQkq) characters
    for (int i = 0; i < 4; i++) {
        if (*fen == ' ') {
            break;
        }
        switch(*fen) {
            case 'K': pos->castle_perm |= WKCA; break;
            case 'Q': pos->castle_perm |= WQCA; break;
            case 'k': pos->castle_perm |= BKCA; break;
            case 'q': pos->castle_perm |= BQCA; break;
            default: break;
        }
        fen++;
    }
    assert((pos->castle_perm >= 0) && (pos->castle_perm <= 15));

    // en passant square
    fen++;
    if (*fen != '-') {
        // Use ASCII trick again to get int vals from characters
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        assert((file >= FILE_A) && (file <= FILE_H));
        assert((rank >= RANK_1) && (rank <= RANK_8));

        pos->en_pas = FR2SQ(file, rank);
    }

    pos->pos_key = generate_pos_key(pos);

    update_lists_material(pos);
}

// Takes a pointer to a board position and resets it
void reset_board(board_t *pos) {
    for (int i = 0; i < BRD_SQ_NUM; i++) {
        pos->pieces[i] = OFFBOARD; // WATCH OUT FOR THIS LATER
        // SHOULD THIS BE NO_SQ INSTEAD? MAYBE GET RID OF OFFBOARD
        // AS AN ENUM ALTOGETHER
        // https://www.youtube.com/watch?v=vF_Td1nABYw&list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg&index=13
    }
    for (int i = 0; i < 64; i++) {
        pos->pieces[SQ120(i)] = EMPTY;
    }
    for (int i = 0; i < 2; i++) {
        pos->big_pce[i] = 0;
        pos->maj_pce[i] = 0;
        pos->min_pce[i] = 0;
        pos->pawns[i] = 0ULL;
        pos->material[i] = 0;
    }
    for (int i = 0; i < 13; i++) {
        pos->pce_num[i] = 0;
    }
    // not set in loop above (WHITE, BLACK, and here BOTH)
    pos->pawns[2] = 0ULL;

    pos->king_sq[WHITE] = NO_SQ;
    pos->king_sq[BLACK] = NO_SQ;

    pos->side = BOTH;
    pos->en_pas = NO_SQ;
    pos->fifty_move = 0;
    pos->ply = 0;
    pos->hist_ply = 0;
    pos->castle_perm = 0;
    pos->pos_key = 0ULL;

    // Reset PV table
    pos->pvtable->p_table = NULL; // Avoid undefined behavior (freeing uninitialized ptr)
    init_pvtable(pos->pvtable);
}

void print_board(const board_t *pos) {
    printf("Board position:\n\n");

    int sq, file, rank, piece;

    for (rank = RANK_8; rank >= RANK_1; rank--) {
        printf("%d   ", rank + 1);
        for (file = FILE_A; file <= FILE_H; file++) {
            sq = FR2SQ(file,rank);
            piece = pos->pieces[sq];
            printf("%3c", pce_char[piece]);
        }
        printf("\n");
    }
    printf("\n    ");
    for (file = FILE_A; file <= FILE_H; file++) {
        // ASCII trick again
        printf("%3c", 'a'+file);
    }
    printf("\nside:%c\n", side_char[pos->side]);
    printf("en passant:%d\n", pos->en_pas);
    printf("castle:%c%c%c%c\n",
        pos->castle_perm & WKCA ? 'K' : '-',
        pos->castle_perm & WQCA ? 'Q' : '-',
        pos->castle_perm & BKCA ? 'k' : '-',
        pos->castle_perm & BQCA ? 'q' : '-');
    printf("pos key:%lx\n", pos->pos_key);
}