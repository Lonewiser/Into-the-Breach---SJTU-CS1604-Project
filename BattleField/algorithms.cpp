#include "algorithms.h"
#include <vector>
#include "Grid.h"
#include <cassert>

using namespace std;

// Data structure for storing squares during path finding
struct SearchSquare {
    SearchSquare(int r, int c, int p) :
        row(r), col(c), pts(p) {
    }

    int row;
    int col;
    int pts;

    // friend std::ostream& operator<< (std::ostream& out, const SearchSquare& sqr);
};

/*
ostream& operator<<(ostream& os, const SearchSquare& sqr)
{
    os << "[row = " << sqr.row << ", col = " << sqr.col << ", mvpt = " << sqr.pts << "]";
    return os;
}
*/

// Find the square at (row, col)
SearchSquare *findSquare(vector<SearchSquare> &squares, int row, int col) {
    for (size_t i = 0; i < squares.size(); i++) {
        SearchSquare &sqr = squares[i];
        if (sqr.row == row && sqr.col == col)
            return &sqr;
    }
    return nullptr;
}

// Find the square with the maximal movement points
int findMaxSquare(const vector<SearchSquare> &squares) {
    assert(squares.size() > 0);

    int maxIndex = 0;
    for (size_t i = 1; i < squares.size(); i++) {
        if (squares[i].pts > squares[maxIndex].pts)
            maxIndex = i;
    }
    return maxIndex;
}

// Update a reachable square if possible
void updateSquare(Grid<bool> &reachable, const Grid<int> &costs,
                  int row, int col, int mvPts,
                  vector<SearchSquare> &unvisited) {
    // No need to check a unattainable position
    // or a square already reachable
    if (!costs.inBounds(row, col)
        || reachable[row][col])
        return;

    // Adjust movement points
    int cost = costs[row][col];
    mvPts -= cost;

    // Update the movement point of unvisited squares
    if (mvPts >= 0) {
        SearchSquare *sqr = findSquare(unvisited, row, col);
        // Update the movement points for unvisited squares
        if (sqr == nullptr)
            unvisited.push_back(SearchSquare(row, col, mvPts));
        else if (mvPts > sqr->pts)
            sqr->pts = mvPts;
    }
}

// Search for reachable points
Grid<bool> searchReachable(const Grid<int> &costs, int row, int col, int mvPts) {
    assert(costs.inBounds(row, col));

    Grid<bool> reachable(costs.numRows(), costs.numCols());

    vector<SearchSquare> unvisited;
    if (mvPts >= 0)
        unvisited.push_back(SearchSquare(row, col, mvPts));

    while (!unvisited.empty()) {
        int maxIndex = findMaxSquare(unvisited);
        SearchSquare pt = unvisited[maxIndex];
        unvisited.erase(unvisited.begin() + maxIndex);
        // unvisited.remove(maxIndex);

        int rMvPts = pt.pts;
        int curRow = pt.row;
        int curCol = pt.col;
        reachable[curRow][curCol] = true;

        // Search four directions
        updateSquare(reachable, costs, curRow - 1, curCol, rMvPts, unvisited); // North
        updateSquare(reachable, costs, curRow + 1, curCol, rMvPts, unvisited); // South
        updateSquare(reachable, costs, curRow, curCol + 1, rMvPts, unvisited); // East
        updateSquare(reachable, costs, curRow, curCol - 1, rMvPts, unvisited); // West
    }

    return reachable;
}

Grid<bool> searchCloseAttackable(const Field &field, int row, int col) {
    Grid<bool> attackable(field.getHeight(), field.getWidth());

    // Check the adjacent squares
    for (int r = row - 1; r <= row + 1; r++) {
        for (int c = col - 1; c <= col + 1; c++) {                     // 遍历九个格子
            if ((r == row || c == col) && attackable.inBounds(r, c)) { // 如果同行列且在范围内
                attackable[r][c] = true;
            }
        }
    }
    return attackable;
}

Grid<bool> searchFarAttackable(const Field &field, int row, int col) {
    Grid<bool> attackable(field.getHeight(), field.getWidth());

    // Check the attack type and set the attackable squares
    for (int r = row + 1; r < field.getHeight(); r++) {
        if (attackable.inBounds(r, col)) {
            attackable[r][col] = true;
        }
        if (field.getTerrain(r, col).getType() != PLAIN || field.getUnit(r, col) != nullptr) {
            break;
        }
    }
    for (int r = row - 1; r >= 0; r--) {
        if (attackable.inBounds(r, col)) {
            attackable[r][col] = true;
        }
        if (field.getTerrain(r, col).getType() != PLAIN || field.getUnit(r, col) != nullptr) {
            break;
        }
    }
    for (int c = col + 1; c < field.getWidth(); c++) {
        if (attackable.inBounds(row, c)) {
            attackable[row][c] = true;
        }
        if (field.getTerrain(row, c).getType() != PLAIN || field.getUnit(row, c) != nullptr) {
            break;
        }
    }
    for (int c = col - 1; c >= 0; c--) {
        if (attackable.inBounds(row, c)) {
            attackable[row][c] = true;
        }
        if (field.getTerrain(row, c).getType() != PLAIN || field.getUnit(row, c) != nullptr) {
            break;
        }
    }
    return attackable;
}