#ifndef INIT_H
#define INIT_H

void all_init();

int files_brd[BRD_SQ_NUM];
int ranks_brd[BRD_SQ_NUM];

U64 set_mask[64];
U64 clear_mask[64];

#endif
