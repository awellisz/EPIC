#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "board.h"

void undo_move(board_t *pos);
bool make_move(board_t *pos, int move);

#endif 