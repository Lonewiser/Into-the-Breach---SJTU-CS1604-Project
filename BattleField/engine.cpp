#include <iomanip>
#include <limits>
#include "engine.h"
#include "terrain.h"
#include "unit.h"
#include "actions.h"
#include "algorithms.h"

using namespace std;

// Forward declaration of auxiliary functions
void printHLine(ostream &os, int n);
string getDpSymbol(dp_mode dp);
Grid<int> getFieldCosts(const Field &field, Unit *u);
bool performAction(Field &field, istream &is, ostream &os, Unit *u, Action act);
bool performMove(ostream &os, istream &is, Field &field, Unit *u);
bool performAttack(ostream &os, istream &is, Field &field, Unit *u);
bool performEnemyAction(Field &field, Unit *u);
int getPositionValue(const Field &field, int row, int col);
int distance(int row1, int col1, int row2, int col2);

// load terrains and units into field
void loadMap(std::istream &is, Field &field) {
    int nt = 0, nu = 0;
    is >> nt >> nu;

    int row, col;
    char type;

    // Load terrains
    for (int i = 0; i < nt; i++) {
        is >> row >> col >> type;
        switch (type) {
        case 'M':
            field.setTerrain(row, col, MOUNTAIN);
            break;
        case 'O':
            field.setTerrain(row, col, OCEAN);
            break;
        default:
            field.setTerrain(row, col, PLAIN);
        }
    }

    // Load units
    for (int i = 0; i < nu; i++) {
        is >> row >> col >> type;
        switch (type) {
        case 'S':
            field.setUnit(row, col, SOLDIER);
            break;
        case 'T':
            field.setUnit(row, col, TANK);
            break;
        case 'B':
            field.setUnit(row, col, BEE);
            break;
        }
    }

    // 清除尾随的一个换行符
    is.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Main loop for playing the game
void play(Field &field, istream &is, ostream &os) {
    while (is) {
        Grid<bool> actionable(field.getHeight(), field.getWidth());
        // 重置己方单位状态 //////////////////////////////////////////
        for (int i = 0; i < field.getHeight(); i++)
            for (int j = 0; j < field.getWidth(); j++) {
                Unit *unit = field.getUnit(i, j);
                if (unit != nullptr) {
                    unit->setMoved(false);
                    unit->setAttacked(false);
                    if (unit->getSide() == true) actionable[i][j] = true; // Mark as actionable
                }
            }

        displayField(os, field); // 打印地图 //////////////////////////////////////////

        // Check if the game is over //////////////////////////////////////////
        int playerCount = 0, enemyCount = 0;
        for (int i = 0; i < field.getHeight(); i++)
            for (int j = 0; j < field.getWidth(); j++) {
                Unit *unit = field.getUnit(i, j);
                if (unit != nullptr) {
                    if (unit->getSide() == true)
                        playerCount++;
                    else
                        enemyCount++;
                }
            }

        if (enemyCount == 0) {
            os << "Won" << endl;
            return; // Player wins
        } else if (playerCount == 0) {
            os << "Failed" << endl;
            return; // Player loses
        }

        // Player's turn ////////////////////////////////////////////////////////
        while (true) {
            // Check if there are any actable units
            bool hasActableUnit = false;
            for (int i = 0; i < field.getHeight(); i++) {
                for (int j = 0; j < field.getWidth(); j++) {
                    Unit *unit = field.getUnit(i, j);
                    if (unit == nullptr || unit->getSide() == false) actionable[i][j] = false;
                    if (unit != nullptr && unit->getSide() == true && (unit->hasMoved() && unit->hasAttacked())) {
                        actionable[i][j] = false; // Mark as not actionable
                    }
                    if (unit != nullptr && unit->getSide() == true && (!unit->hasMoved() || !unit->hasAttacked())) {
                        actionable[i][j] = true; // Mark as actionable
                        hasActableUnit = true;
                    }
                }
            }
            if (!hasActableUnit) {
                os << "No more actable units." << endl;
                break; // exit the loop if no actable units
            }

            // Display the field with actionable units
            displayField(os, field, actionable, DP_ACTIONABLE);

            // Ask if the player wants to skip their turn
            char skip_choice;
            os << "End this turn (y,n)?" << endl;
            is >> skip_choice;
            if (skip_choice == 'y' || skip_choice == 'Y') {
                break;
            }

            // select unit
            Unit *u = nullptr;
            int row, col;
            while (true) {
                os << "Please select a unit:" << endl;
                is >> row >> col;
                if (field.getUnit(row, col) == nullptr) {
                    os << "No unit at (" << row << ", " << col << ")!" << endl;
                } else if (field.getUnit(row, col)->getSide() == false) {
                    os << "Unit at (" << row << ", " << col << ") is an enemy!" << endl;
                } else if (!actionable[row][col]) {
                    os << "Unit at (" << row << ", " << col << ") is not actable!" << endl;
                } else {
                    break; // valid unit selected
                }
            }
            u = field.getUnit(row, col);

            // select action
            vector<Action> actionList = getActions(u);

            int act;
            for (int i = 0; i < actionList.size(); i++) {
                switch (actionList[i]) {
                case MOVE:
                    os << i + 1 << ".Move ";
                    break;
                case ATTACK:
                    os << i + 1 << ".Attack ";
                    break;
                case SKIP:
                    os << i + 1 << ".Skip ";
                    break;
                }
            }
            os << endl;
            while (true) {
                os << "Select your action:" << endl;

                is >> act;
                if (act > 0 && act < actionList.size() + 1) break;
                // else if invalid action
                os << "Invalid action!" << endl;
            }
            Action selectedAction = actionList[act - 1];

            // perform action
            // A function called performAction is defined below.
            // You can use it or define your own version.
            bool success = performAction(field, is, os, u, selectedAction);
        }

        // Enemy's turn ////////////////////////////////////////////////////////
        for (int i = 0; i < field.getHeight(); i++) {    // 遍历row，row小的单位先行动
            for (int j = 0; j < field.getWidth(); j++) { // 遍历col，row相同时col小的单位先行动
                Unit *unit = field.getUnit(i, j);
                if (unit != nullptr && unit->getSide() == false) { // Enemy unit
                    if (unit->hasMoved()) continue;
                    // Perform actions fors the enemy unit
                    performEnemyAction(field, unit);
                }
            }
        }
    }
}

// Display the field on the out stream os
void displayField(ostream &os, const Field &field, const Grid<bool> &grd, dp_mode dp) {
    int height = field.getHeight();
    int width = field.getWidth();
    string dp_symbol = getDpSymbol(dp);

    os << endl;
    // Print the x coordinates
    os << "  ";
    for (int i = 0; i < width; i++)
        os << setw(3) << i << " ";
    os << endl;

    printHLine(os, width);
    for (int i = 0; i < height; i++) {
        os << setw(2) << i;
        for (int j = 0; j < width; j++) {
            os << '|';
            const Unit *u = field.getUnit(i, j);
            Terrain t = field.getTerrain(i, j);
            string sym;
            int width = 3;
            if (grd.inBounds(i, j) && grd[i][j]) {
                os << setw(1) << dp_symbol;
                width -= 1;
            }

            if (u != nullptr)
                sym += u->getSymbol();
            else
                sym += t.getSymbol();
            os << setw(width) << sym;
        }
        os << '|' << endl;
        printHLine(os, width);
    }
    os << endl;
}

// Print the horizontal line
void printHLine(ostream &os, int n) {
    os << "  ";
    for (int i = 0; i < n; i++)
        os << "+---";
    os << "+" << endl;
}

// symbol used in displayField
string getDpSymbol(dp_mode dp) {
    if (dp == DP_MOVE) return ".";
    if (dp == DP_ATTACK) return "*";
    if (dp == DP_ACTIONABLE) return "+";

    return " ";
}

bool performAction(Field &field, istream &is, ostream &os, Unit *u, Action act) {
    switch (act) {
    case MOVE:
        return performMove(os, is, field, u);
        break;
    case ATTACK:
        return performAttack(os, is, field, u);
        break;
    case SKIP:
        return true;
        break;
    default:
        os << "(Action not implemented yet)" << endl;
        return false;
    }
}

// Perform the move action
bool performMove(ostream &os, istream &is, Field &field, Unit *u) {
    // Display the reachable points
    Grid<bool> grd =
        searchReachable(getFieldCosts(field, u), u->getRow(), u->getCol(), u->getMovPoints());

    displayField(os, field, grd, DP_MOVE);

    // Ask for the target coordinate
    int trow, tcol;
    while (true) {
        os << "Please enter your destination:" << endl;
        is >> trow >> tcol;

        if (grd.inBounds(trow, tcol) && grd[trow][tcol]) break;
        // else if the target coordinate is not reachable
        os << "Not a valid destination" << endl;
    }
    u->setMoved(true); // Mark the unit as moved
    return field.moveUnit(u->getRow(), u->getCol(), trow, tcol);
}

// Perform the attack action
bool performAttack(ostream &os, istream &is, Field &field, Unit *u) {
    // Display the reachable points
    Grid<bool> grd;
    if (u->getType() == TANK)
        grd = searchFarAttackable(field, u->getRow(), u->getCol());
    else
        grd = searchCloseAttackable(field, u->getRow(), u->getCol());

    displayField(os, field, grd, DP_ATTACK);

    // Ask for the target coordinate
    int trow, tcol;
    while (true) {
        os << "Please enter your target: " << endl;
        is >> trow >> tcol;

        if (grd.inBounds(trow, tcol) && grd[trow][tcol]) break;
        // else if the target coordinate is not reachable
        os << "Not a valid target" << endl;
    }
    u->setAttacked(true); // Mark the unit as attacked
    return field.attackUnit(u, trow, tcol);
}

// Perform Enemy's action
bool performEnemyAction(Field &field, Unit *u) {
    // Move
    Grid<bool> grd =
        searchReachable(getFieldCosts(field, u), u->getRow(), u->getCol(), u->getMovPoints());

    // Find the best position to move
    int bestValue = -1;
    int bestRow = -1, bestCol = -1;
    for (int i = 0; i < grd.numRows(); i++) {
        for (int j = 0; j < grd.numCols(); j++) {
            if (grd[i][j]) {
                int value = getPositionValue(field, i, j);
                if (value > bestValue) { // If the position is valuable
                    bestRow = i;
                    bestCol = j;
                    bestValue = value;
                }
            }
        }
    }
    u->setMoved(true); // Mark the unit as moved
    if (bestValue == -1) return false; 
    field.moveUnit(u->getRow(), u->getCol(), bestRow, bestCol);

    // Attack
    Grid<bool> grd2 = searchCloseAttackable(field, u->getRow(), u->getCol());

    Unit *targetToAttack = nullptr;
    for (int i = 0; i < grd2.numRows(); i++) {
        for (int j = 0; j < grd2.numCols(); j++) {
            if (grd2[i][j] && field.getUnit(i, j) != nullptr) { // If the cell is reachable and has a unit
                Unit *target = field.getUnit(i, j);
                if (target->getSide() == true) { // If the target is an enemy
                    targetToAttack = target;
                    break;
                }
            }
        }
        if (targetToAttack != nullptr) break; // Found a target to attack
    }
    if (targetToAttack != nullptr) field.attackUnit(u, targetToAttack->getRow(), targetToAttack->getCol());
    u->setAttacked(true); // Mark the unit as attacked
    return true;          // Successfully performed the enemy action
}

// Convert field to costs
// The cost should depend on the terrain type and unit type
Grid<int> getFieldCosts(const Field &field, Unit *u) {
    int h = field.getHeight();
    int w = field.getWidth();
    Grid<int> costs(h, w);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            switch (u->getType()) {
            case SOLDIER:
            case TANK:
                if (u->getRow() == i && u->getCol() == j) { // 当前单位
                    costs[i][j] = 0;
                } else if (field.getUnit(i, j) == nullptr &&              // 无单位
                           (field.getTerrain(i, j).getType() == PLAIN)) { // 或者是平原
                    costs[i][j] = 1;
                } else { // 有单位或山或海
                    costs[i][j] = 100;
                }
                break;
            case BEE:
                if (u->getRow() == i && u->getCol() == j) { // 当前单位
                    costs[i][j] = 0;
                } else if (field.getUnit(i, j) == nullptr &&                 // 无单位
                           (field.getTerrain(i, j).getType() == PLAIN        // 且是平原
                            || field.getTerrain(i, j).getType() == OCEAN)) { // 或者是海洋
                    costs[i][j] = 1;
                } else { // 有单位或山或森林
                    costs[i][j] = 100;
                }
                break;
            }
        }

    return costs;
}

int getPositionValue(const Field &field, int row, int col) {
    int h = field.getHeight();
    int w = field.getWidth();

    int min_distance = 999;
    // 遍历所有格子
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (field.getUnit(i, j) != nullptr && field.getUnit(i, j)->getSide() == true) {
                // 如果是己方单位，计算距离
                int d = distance(row, col, i, j);
                if (d < min_distance) {
                    min_distance = d; // 更新最小距离
                }
            }
        }
    }
    if (min_distance == 999) return -1; // 如果没有找到己方单位，返回-1
    return 999 - min_distance;
}

int distance(int row1, int col1, int row2, int col2) {
    return abs(row1 - row2) + abs(col1 - col2); // 曼哈顿距离
}