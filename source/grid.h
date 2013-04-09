#ifndef _GRID_H
#define _GRID_H

#include "tile.h"
#include "tower.h"
#include "rendering.h"
#include "point.h"
#include "grass.h"

class Grid 
{
	Tile** tiles; //1D array of pointers to static terrain/objects
public:
	Grid();
	~Grid();
	void buildGrass(int x, int y);
	void buildSpawn(int x, int y);
	Tower* buildTower(int x, int y);
	void buildWater(int x, int y);
	void buildExit(int x, int y);
	Tile* get(int x, int y);
	Tile* get(Point &p);
	const Tile & get(int x, int y) const;
	void releaseTile(int x, int y);
	void render() const;
	void notifyTileExit(const Point &p, int mobId);
	void notifyTileEnter(const Point &p, int mobId);
	bool isGrassAt(int x, int y) const;
	Tile* at(int x, int y) const;
};

inline Tile* Grid::at(int x, int y) const
{
	return tiles[x + y*GRID_COLUMNS];
}

inline void Grid::releaseTile(int x, int y)
{
	delete at(x,y);
}

inline Tower* Grid::buildTower(int x, int y)
{
	releaseTile(x, y);
	Tower *t = new Tower(x, y);
	tiles[x + y*GRID_COLUMNS] = t;
	return t;
}

inline void Grid::buildGrass(int x, int y)
{
	tiles[x + y*GRID_COLUMNS] = new Grass();
}

inline void Grid::buildWater(int x, int y)
{
	at(x, y)->setColor(WATER);
}

inline void Grid::buildSpawn(int x, int y)
{
	releaseTile(x, y);
	tiles[x + y*GRID_COLUMNS] = new Spawn();
}

inline void Grid::buildExit(int x, int y)
{
	releaseTile(x, y);
	tiles[x + y*GRID_COLUMNS] = new Exit();
}

#endif //_GRID_H
