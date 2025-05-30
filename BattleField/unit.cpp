#include <string>
#include <cassert>
#include "unit.h"

using namespace std;

// Constructor
Unit::Unit(UnitType t, bool sd, int row, int col) :
    type(t), side(sd), urow(row), ucol(col) {
}

// Get the symbol of the unit
string Unit::getSymbol() const {
    switch (type) {
    case SOLDIER:
        return side ? "S" : "s";
    case TANK:
        return side ? "T" : "t";
    }

    return "";
}

// Get the coordinate of the current unit
int Unit::getRow() const {
    return urow;
}

int Unit::getCol() const {
    return ucol;
}

// Set the coordinates
void Unit::setCoord(int row, int col) {
    urow = row;
    ucol = col;
}

// unit performs move action
void Unit::move(int row, int col) {
    // setCoord(row, col);
}

bool Unit::getSide() const {
    return side;
}

// Get movement point
int Unit::getMovPoints() const {
    switch (type) {
    case SOLDIER:
        return 3;
    default:
        return 0;
    }
}