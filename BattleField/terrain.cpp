#include <cassert>
#include "terrain.h"

// Constructor
Terrain::Terrain() :
    ttype(PLAIN) {
}

std::string Terrain::getSymbol() {
    switch (ttype) {
    case PLAIN:
        return "  ";
    case MOUNTAIN:
        return "/\\";
    case OCEAN:
        return "~~";
    }

    // assert(false);
    return "";
}

void Terrain::setType(TerrainType type) {
    this->ttype = type;
}

TerrainType Terrain::getType() const {
    return ttype;
}