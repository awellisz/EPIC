#ifndef HASHKEYS_H
#define HASHKEYS_H

#include "board.h"

U64 rand_U64(void);
U64 generate_pos_key(const board_t *pos);

#endif 
