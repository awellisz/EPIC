#ifndef PROTOTYPES_H
#define PROTOTYPES_H

#include <stdlib.h>
// to disable assertions:
// #define NDEBUG 
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>


// init.c
void all_init();

// bitboards.c
void print_bitboard(U64 bb);
int pop_bit(U64 *bb);
int count_bits(U64 bb);
void set_bit(U64 *bb, int sq);
void clear_bit(U64 *bb, int sq);

// hashkeys.c
U64 rand_U64(void);
U64 generate_pos_key(const board_t *pos);

// board.c
void reset_board(board_t *pos);
void parse_fen(char *fen, board_t *pos);
void print_board(const board_t *pos);
void update_lists_material(board_t *pos);
bool check_board(const board_t *pos);

// attack.c
bool sq_attacked(const int sq, const int side, const board_t *pos);

// io.c
char *sq_to_str(const int sq);
char *move_to_str(const int move);
void print_move_list(const movelist_t *list);

// validate.c
bool sq_on_board(const int sq);
bool side_valid(const int side);
bool file_rank_valid(const int fr);
bool piece_valid_empty(const int pce);
bool piece_valid(const int pce);

// movegen.c
void generate_all_moves(const board_t *pos, movelist_t *list);

// makemove.c
void undo_move(board_t *pos);
bool make_move(board_t *pos, int move);


#endif