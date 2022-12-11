#ifndef IO_H 
#define IO_H

char *sq_to_str(const int sq);
char *move_to_str(const int move);
void print_move_list(const movelist_t *list);
int parse_move(char *movep, board_t *pos);

#endif 
