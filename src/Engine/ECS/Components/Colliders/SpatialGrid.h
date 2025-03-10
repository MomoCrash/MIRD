#pragma once

#include <unordered_map>
#include <vector>

struct CellCoords {
    int x, y, z;

    bool operator==(const CellCoords& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct CellHash {
    std::size_t operator()(const CellCoords& key) const {
        return std::hash<int>()(key.x) ^ std::hash<int>()(key.y) ^ std::hash<int>()(key.z);
    }
};

class SpatialGrid {
public:
    SpatialGrid(float cellSize, int minGridSizeX, int minGridSizeY, int minGridSizeZ, int maxGridSizeX, int maxGridSizeY, int maxGridSizeZ);
    ~SpatialGrid();

    void UpdateEntity(Entity* entity);

    bool IsValidCell(const CellCoords& coords) const;

    CellCoords GetCellCoords(float x, float y, float z) const;

    void GetNeighboringEntities(int cellX, int cellY, int cellZ, std::vector<Entity*>& outEntities);

    std::vector<Entity*> GetEntitiesInCell(int x, int y, int z);

    std::unordered_map<CellCoords, std::vector<Entity*>, CellHash> GetAllCells() { return mCells; }

    // Récupère la taille de la grille
    int GetGridSizeX() const { return mGridSizeX; }
    int GetGridSizeY() const { return mGridSizeY; }
    int GetGridSizeZ() const { return mGridSizeZ; }
    float GetCellSize() const { return mCellSize; }

private:
    float mCellSize; // Taille d'une cellule
    int mMinX, mMinY, mMinZ; // min de la grille
    int mMaxX, mMaxY, mMaxZ; // max de la grille
    int mGridSizeX, mGridSizeY, mGridSizeZ; // Taille de la grille en cell
    std::unordered_map<CellCoords, std::vector<Entity*>, CellHash> mCells; 
};