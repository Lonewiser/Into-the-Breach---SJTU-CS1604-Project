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
    units[row][col] = new Unit(unitType, true, row, col); // Create a new unit
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

    return true;
}