#ifndef BOARD_H 
#define BOARD_H 

#include "defs.h"

void reset_board(board_t *pos);
void parse_fen(char *fen, board_t *pos);
void print_board(const board_t *pos);
void update_lists_material(board_t *pos);
bool check_board(const board_t *pos);

#endif
