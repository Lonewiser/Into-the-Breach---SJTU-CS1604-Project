#include <string>
#include <cassert>
#include "unit.h"
#include "actions.h"

using namespace std;

// Constructor
Unit::Unit(UnitType t, bool sd, int row, int col) :
    type(t), side(sd), urow(row), ucol(col) {
    switch (type) {
    case SOLDIER:
        hp = 2;
        break;
    case TANK:
        hp = 3;
        break;
    case BEE:
        hp = 2;
        break;
    case FLIGHTER:
        hp = 2;
        break;
    case HYDRAULISK:
        hp = 1;
        break;

    default:
        break;
    }
}

// Get the symbol of the unit
string Unit::getSymbol() const {
    string symbol = "";
    switch (type) {
    case SOLDIER:
        symbol = side ? "S" : "s";
        break;
    case TANK:
        symbol = side ? "T" : "t";
        break;
    case BEE:
        symbol = side ? "B" : "b";
        break;
    case FLIGHTER:
        symbol = side ? "F" : "f";
        break;
    case HYDRAULISK:
        symbol = side ? "H" : "h";
        break;
    }
    symbol += to_string(hp);

    return symbol;
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

// Being attacked
void Unit::receiveDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
        // Handle unit destruction logic here, e.g., remove from field
        hp = 0; // Ensure hp does not go below 0
    }
}

bool Unit::getSide() const {
    return side;
}

// Get movement point
int Unit::getMovPoints() const {
    switch (type) {
    case SOLDIER:
        return 3;
    case TANK:
        return 2;
    case BEE:
        return 3;
    case FLIGHTER:
        return 5;
    case HYDRAULISK:
        return 3;
    default:
        return 0;
    }
}

// Get the unit's attack points
int Unit::getAttackPoints() const {
    switch (type) {
    case SOLDIER:
        return 1;
    case TANK:
        return 1;
    case BEE:
        return 3;
    case FLIGHTER:
        return 2;
    case HYDRAULISK:
        return 2;
    default:
        return 0; // Default to 0 for unknown types
    }
}

// Check if the unit is alive
bool Unit::isAlive() const {
    return hp > 0;
}

// Get the unit type
UnitType Unit::getType() const {
    return type;
}

// Check if the unit has moved
bool Unit::hasMoved() const {
    return moved;
}

// Check if the unit has attacked
bool Unit::hasAttacked() const {
    return attacked;
}

// Set the moved status
void Unit::setMoved(bool m) {
    moved = m;
}

// Set the attacked status
void Unit::setAttacked(bool a) {
    attacked = a;
}