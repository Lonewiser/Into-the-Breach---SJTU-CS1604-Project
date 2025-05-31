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
        displayField(os, field);

        // select unit
        Unit *u = nullptr;
        int row, col;
        while (true) {
            os << "Please select a unit:" << endl;
            is >> row >> col;
            if (field.getUnit(row, col) == nullptr) {
                os << "No unit at (" << row << ", " << col << ")!" << endl;
            } else if (field.getUnit(row, col)->getSide() == false) {
                os << " Unit at (" << row << ", " << col << ") is an enemy!" << endl;
            } else {
                break; // valid unit selected
            }
        }
        u = field.getUnit(row, col);

        // select action
        vector<Action> actionList = getActions(u);
        // if (isAllSkip(actionList)) {
        //     os << "No more actable units in this turn!" << endl;
        //     continue; // skip to the next loop
        // }

        int act;
        while (true) {
            for (int i = 0; i < actionList.size(); i++) {
                switch (actionList[i]) {
                case MOVE:
                    os << i + 1 << ". Move ";
                    break;
                case ATTACK:
                    os << i + 1 << ". Attack ";
                    break;
                case SKIP:
                    os << i + 1 << ". Skip ";
                    break;
                }
            }
            os << endl
               << "Select your action: " << endl;

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

        // The following line is needed in task 1 and task2.
        // It avoids entering the next loop
        // when reaches the end of the input data.
        // Remove it in task 3 and task 4.
        if (is.eof()) break;
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

    return " ";
}

bool performAction(Field &field, istream &is, ostream &os, Unit *u, Action act) {
    switch (act) {
    case MOVE:
        return performMove(os, is, field, u);

    case ATTACK:
        return performAttack(os, is, field, u);

    case SKIP:
        return true;

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
        os << "Please enter your destination: " << endl;
        is >> trow >> tcol;

        if (grd.inBounds(trow, tcol) && grd[trow][tcol]) break;
        // else if the target coordinate is not reachable
        os << "Not a valid destination" << endl;
    }
    return field.moveUnit(u->getRow(), u->getCol(), trow, tcol);
}

bool performAttack(ostream &os, istream &is, Field &field, Unit *u) {
    // Display the reachable points
    Grid<bool> grd;
    if (u->getType() == SOLDIER || u->getType() == BEE)
        grd = searchCloseAttackable(field, u->getRow(), u->getCol());
    else
        grd = searchFarAttackable(field, u->getRow(), u->getCol());

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
    return field.attackUnit(u, trow, tcol);
}

// Convert field to costs
// The cost should depend on the terrain type and unit type
Grid<int> getFieldCosts(const Field &field, Unit *u) {
    int h = field.getHeight();
    int w = field.getWidth();
    Grid<int> costs(h, w);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            // switch (field.getUnit(i, j)->getType()) {
            //    case SOLDIER:
            if (u->getRow() == i && u->getCol() == j) { // 当前单位
                costs[i][j] = 0;
            } else if (field.getUnit(i, j) == nullptr &&            // 无单位
                       field.getTerrain(i, j).getType() == PLAIN) { // 且是平原
                costs[i][j] = 1;
            } else { // 有单位或非平原
                costs[i][j] = 100;
            }
            //    break;
            //    case TANK:
            //    break;
            // }
        }

    return costs;
}
