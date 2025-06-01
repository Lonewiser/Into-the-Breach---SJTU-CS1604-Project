#include <cassert>
#include "field.h"

// Constructor
Field::Field(int h, int w) :
    units(h, w), terrains(h, w) {
}

// Destructor
// Reclaim all the units
Field::~Field() {
    for (size_t i = 0; i < units.numRows(); i++)
        for (size_t j = 0; j < units.numCols(); j++)
            if (units[i][j] != nullptr)
                delete units[i][j];
}

// Get the height of the field
size_t Field::getHeight() const {
    return units.numRows();
}

// Get the width of the field
size_t Field::getWidth() const {
    return units.numCols();
}

// Get the unit at row and col
Unit *Field::getUnit(int row, int col) const {
    assert(units.inBounds(row, col));

    return units[row][col];
}

// Set the unit at row and col
void Field::setUnit(int row, int col, UnitType unitType) {
    assert(units.inBounds(row, col));

    if (units[row][col] != nullptr) {
        delete units[row][col]; // Delete the existing unit
    }
    if (unitType == SOLDIER || unitType == TANK)
        units[row][col] = new Unit(unitType, true, row, col); // Create a new unit
    else
        units[row][col] = new Unit(unitType, false, row, col); // Create a new unit
}

// Get the terrain at row and col
Terrain Field::getTerrain(int row, int col) const {
    return terrains[row][col];
}

// Set the terrain at row and col
void Field::setTerrain(int row, int col, TerrainType terrainType) {
    assert(terrains.inBounds(row, col));

    terrains[row][col].setType(terrainType);
}

bool Field::moveUnit(int srow, int scol, int trow, int tcol) {
    assert(units.inBounds(srow, scol));
    assert(units.inBounds(trow, tcol));

    if (units[trow][tcol] != nullptr) {
        // Target cell is already occupied
        return false;
    }

    // Move the unit from (srow, scol) to (trow, tcol)
    Unit *unit = units[srow][scol];
    assert(unit != nullptr);     // Ensure there is a unit to move
    units[trow][tcol] = unit;    // Place the unit in the new position
    units[srow][scol] = nullptr; // Clear the old position
    unit->setCoord(trow, tcol);  // Update the unit's coordinates

    return true;
}

bool Field::attackUnit(Unit *u, int trow, int tcol) {
    assert(units.inBounds(trow, tcol));
    assert(u != nullptr);

    UnitType utype = u->getType();
    Unit *target = units[trow][tcol];

    switch (utype) {
    case SOLDIER:
    case BEE:
        if (target != nullptr) {
            target->receiveDamage(u->getAttackPoints());
        }
        break;

    case TANK:
        if (target == nullptr && terrains[trow][tcol].getType() == MOUNTAIN) {
            terrains[trow][tcol].setType(PLAIN); // TANK can destroy MOUNTAIN
        }
        if (target != nullptr) {
            target->receiveDamage(u->getAttackPoints());
            beatBack(u->getRow(), u->getCol(), target);
        }
        break;
    default:
        break;
    }

    // 清除血量为0的单位
    for (size_t i = 0; i < units.numRows(); i++) {
        for (size_t j = 0; j < units.numCols(); j++) {
            if (units[i][j] != nullptr && !units[i][j]->isAlive()) {
                delete units[i][j];    // Delete the dead unit
                units[i][j] = nullptr; // Clear the position
            }
        }
    }

    return true;
}

void Field::beatBack(int srow, int scol, Unit *u) {
    assert(u != nullptr);
    int trow = u->getRow();
    int tcol = u->getCol();
    int direction = 0;
    int drow[] = {-1, 1, 0, 0}; // UP, DOWN, LEFT, RIGHT
    int dcol[] = {0, 0, -1, 1}; // UP, DOWN, LEFT, RIGHT

    // 计算击退方向
    if (tcol == scol && trow < srow) {
        direction = 0; // UP
    } else if (tcol == scol && trow > srow) {
        direction = 1; // DOWN
    } else if (trow == srow && tcol < scol) {
        direction = 2; // LEFT
    } else if (trow == srow && tcol > scol) {
        direction = 3; // RIGHT
    } else {
        return; // Invalid beat back direction
    }

    int newRow = trow + drow[direction];
    int newCol = tcol + dcol[direction];

    if (!units.inBounds(newRow, newCol)) return; // Out of bounds
    if (units[newRow][newCol] != nullptr) {
        u->receiveDamage(1);                       // If the new position is occupied, the unit takes 1 damage
        getUnit(newRow, newCol)->receiveDamage(1); // The unit in the new position also takes 1 damage
        return;
    }

    TerrainType terrainType = terrains[newRow][newCol].getType(); // 获取击退后的地形类型
    switch (terrainType) {
    case PLAIN:
        // Move the unit back to the next square in the direction of the attack
        moveUnit(trow, tcol, newRow, newCol); // Move the unit to the new position
        break;
    case MOUNTAIN:
        u->receiveDamage(1);                     // If the terrain is MOUNTAIN, the unit takes 1 damage
        terrains[newRow][newCol].setType(PLAIN); // MOUNTAIN becomes PLAIN
        break;
    case OCEAN:
        if (u->getType() == SOLDIER || u->getType() == TANK) {
            u->receiveDamage(999); // get destroyed
        } else {
            moveUnit(trow, tcol, newRow, newCol); // Move the unit to the new position
        }
        break;
    case FOREST:
        if (u->getType() == SOLDIER || u->getType() == TANK) {
            moveUnit(trow, tcol, newRow, newCol); // Move the unit to the new position
        }
        break;
    default:
        break;
    }
}