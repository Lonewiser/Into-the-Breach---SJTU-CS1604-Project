#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include <string>

// TerrainType
enum TerrainType { PLAIN,
                   MOUNTAIN,
                   OCEAN,
                   FOREST,
};

class Terrain {
public:
    Terrain();

    std::string getSymbol();

    void setType(TerrainType type);
    TerrainType getType() const;

private:
    TerrainType ttype;
};

#endif // TERRAIN_H_INCLUDED
