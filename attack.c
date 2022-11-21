// attack.c
#include <stdbool.h>
#include <assert.h>

#include "defs.h"
#include "functions.h"

/* Knight, rook, bishop, and king direction arrays
Arrays represent the possible directions each piece type can attack from
e.g. a rook can be to the left (-1), to the right (+1), up (-10), or down (+10)
To check if a square is being attacked by a rook, iterate over these
possible directions and check if there is a rook (or, in this case, a queen, 
since a queen can also move like a rook). The same applies for the other pieces: 
the bishop array represents motion diagonally; the knight array represents the
knight's L-shaped motion; etc.

See the board representation for more on why these indices are correct
*/
const int kn_dir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int rk_dir[4] = {-1, -10, 1, 10};
const int bi_dir[4] = {-9, -11, 11, 9};
const int ki_dir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

/* sq_attacked: determines whether a square is under attack by looping over 
 *              all possible attack vantage squares and checking the pieces
 *
 * sq: square we're interested in (is sq under attack?)
 * side: the attacking side
 * pos: pointer to current board position
 *
 * returns: true or false (whether sq is under attack)
 */
bool sq_attacked(const int sq, const int side, const board_t *pos) {
    int pce, i, t_sq, dir;

    assert(sq_on_board(sq));
    assert(side_valid(side));
    assert(check_board(pos));


    // Pawns
    // Well-defined: pawns only attack forward diagonally (-11 or -9) for white,
    //   +11 and +9 direction for black
    if (side == WHITE) {
        if (pos->pieces[sq-11] == wP || pos->pieces[sq-9] == wP) {
            return true;
        }
    } else {
        if (pos->pieces[sq+11] == bP || pos->pieces[sq+9] == bP) {
            return true;
        }
    }

    // Knights
    // Loop through all possible squares this square could be attacked from
    //  by a knight (and check if there's a knight there)
    for (i = 0; i < 8; i++) {
        pce = pos->pieces[sq + kn_dir[i]];
        if (IsN(pce) && piece_col[pce] == side) {
            return true;
        }
    }

    // Rooks and queens
    for (i = 0; i < 4; i++) {
        dir = rk_dir[i];
        // target square to check
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        // Keep going until off board
        while (pce != OFFBOARD) {
            if (pce != EMPTY) {
                if (IsRQ(pce) && piece_col[pce] == side) {
                    return true;
                }
                // If you hit a piece but it's not R or Q, exit loop
                break;
            }
            // Iterate down board in the specified direction
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    // Bishops and queens
    for (i = 0; i < 4; i++) {
        dir = bi_dir[i];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while (pce != OFFBOARD) {
            if (pce != EMPTY) {
                if (IsBQ(pce) && piece_col[pce] == side) {
                    return true;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    // King
    // Only move 1 square, so just need to check the 8 squares
    for (i = 0; i < 8; i++) {
        pce = pos->pieces[sq + ki_dir[i]];
        if (IsK(pce) && piece_col[pce] == side) {
            return true;
        }
    }

    return false;

}