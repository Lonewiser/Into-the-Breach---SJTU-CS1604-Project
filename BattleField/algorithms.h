#ifndef ALGORITHMS_H_INCLUDED
#define ALGORITHMS_H_INCLUDED

/**** Algorithms for the game ****/
#include "Grid.h"
#include "field.h"

/** Path finding algorithm **/

// Given movement points (pts), calculate
// which squares can be reached starting from (row, col)
Grid<bool> searchReachable(const Grid<int> &costs, int row, int col, int pts);

Grid<bool> searchCloseAttackable(const Field &field, int row, int col);
Grid<bool> searchFarAttackable(const Field &field, int row, int col);

#endif // ALGORITHMS_H_INCLUDED
