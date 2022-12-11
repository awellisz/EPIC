#ifndef VALIDATE_H 
#define VALIDATE_H

bool sq_on_board(const int sq);
bool side_valid(const int side);
bool file_rank_valid(const int fr);
bool piece_valid_empty(const int pce);
bool piece_valid(const int pce);

#endif